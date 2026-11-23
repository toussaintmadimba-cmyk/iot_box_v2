#include "gsm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace djua {
namespace {
constexpr const char* commands[] = {
    "AT\r", "ATE0\r", "AT+CMEE=2\r", "AT+CPIN?\r", "AT+CEREG?\r", "AT+CREG?\r",
    "AT+CSMS?\r", "AT+CSCA?\r", "AT+CMGF=1\r", "AT+CSCS=\"IRA\"\r", "AT+CSMP=17,167,0,0\r"
};
int registrationStatus(const char* data) {
    char* end = nullptr;
    long value = strtol(data, &end, 10);
    if (end == data || value < 0 || value > 10) return -1;
    if (*end && *end != ',') return -1;
    if (*end == ',') {
        const char* second = end + 1; while (*second == ' ') ++second;
        // Detailed URC has a quoted area identifier, query has numeric <stat>.
        if (*second != '"') {
            value = strtol(second, &end, 10);
            if (end == second || value < 0 || value > 10) return -1;
            if (*end && *end != ',') return -1;
        }
    }
    return static_cast<int>(value);
}
}
void GsmEngine::begin(bool configured, uint32_t now) {
    state_ = configured ? GsmState::Backoff : GsmState::Disabled;
    since_ = now - retryInterval_; result_ = GsmResult::None; fault_ = GsmFault::None;
    registered_ = simReady_ = polling_ = false; used_ = 0;
    lteRegistered_ = csRegistered_ = smsSupported_ = serviceCentre_ = false; errorCode_ = -1;
}
void GsmEngine::startCommand(uint32_t now) {
    state_ = GsmState::Initializing; since_ = now; used_ = 0;
    const char* command = commands[step_];
    // Queries must supply fresh evidence, not a previous successful response.
    {
        if (step_ == 4) lteRegistered_ = false;
        if (step_ == 5) csRegistered_ = false;
        if (step_ == 6) smsSupported_ = false;
        if (step_ == 7) serviceCentre_ = false;
    }
    if (!io_.write(command, strlen(command))) fail(now, GsmFault::WriteError);
}
void GsmEngine::fail(uint32_t now, GsmFault fault) {
    fault_ = fault;
    // ESC abandonne une saisie de SMS eventuellement encore ouverte.
    const char escape = 27; io_.write(&escape, 1);
    if (busy()) result_ = GsmResult::Failed;
    state_ = GsmState::Backoff; since_ = now; used_ = 0;
    registered_ = simReady_ = polling_ = false;
    lteRegistered_ = csRegistered_ = smsSupported_ = serviceCentre_ = false;
}
void GsmEngine::line(uint32_t now) {
    line_[used_] = 0; used_ = 0;
    if (!strcmp(line_, "+CPIN: READY")) simReady_ = true;
    if (!strncmp(line_, "+CREG:", 6)) {
        const int registration = registrationStatus(line_ + 6);
        csRegistered_ = registration == 1 || registration == 5 ||
            (registration == 6 || registration == 7);
        registered_ = csRegistered_ || lteRegistered_;
    }
    if (!strncmp(line_, "+CEREG:", 7)) {
        const int registration = registrationStatus(line_ + 7);
        lteRegistered_ = registration == 1 || registration == 5;
        registered_ = csRegistered_ || lteRegistered_;
    }
    if (!strncmp(line_, "+CSMS:", 6)) {
        int service = -1, mt = -1, mo = -1, bm = -1;
        smsSupported_ = sscanf(line_ + 6, " %d,%d,%d,%d", &service, &mt, &mo, &bm) == 4 && mo == 1;
    }
    if (!strncmp(line_, "+CSCA:", 6)) {
        serviceCentre_ = false;
        const char* first = strchr(line_ + 6, '"');
        const char* last = first ? strchr(first + 1, '"') : nullptr;
        char centre[18] = {};
        if (last && last > first + 1 && last - first - 1 < static_cast<int>(sizeof(centre))) {
            memcpy(centre, first + 1, static_cast<size_t>(last - first - 1));
            // National SMSC addresses can be returned without '+'.
            serviceCentre_ = true;
            for (size_t i = centre[0] == '+' ? 1 : 0; centre[i]; ++i)
                if (centre[i] < '0' || centre[i] > '9') serviceCentre_ = false;
            if (!strcmp(centre, "+")) serviceCentre_ = false;
        }
    }
    if (!strncmp(line_, "+CMGS:", 6) && state_ == GsmState::Submitting) {
        const char* p = line_ + 6; while (*p == ' ') ++p;
        sawCmgs_ = *p >= '0' && *p <= '9';
    }
    if (!strcmp(line_, "ERROR") || !strncmp(line_, "+CMS ERROR", 10) ||
        !strncmp(line_, "+CME ERROR", 10) || !strcmp(line_, "NORMAL POWER DOWN") ||
        (!strcmp(line_, "RDY"))) {
        // Un module 2G (ex: SIM800C) ne supporte pas la commande LTE AT+CEREG? (etape 4) :
        // on ignore l'erreur et on passe directement a AT+CREG? (etape 5).
        if (state_ == GsmState::Initializing && step_ == 4) {
            lteRegistered_ = false;
            ++step_;
            startCommand(now);
            return;
        }
        const char* colon = strchr(line_, ':');
        if (colon) { char* end = nullptr; long code = strtol(colon + 1, &end, 10);
            if (end != colon + 1 && code >= 0 && code <= 65535) errorCode_ = static_cast<int>(code); }
        fail(now, GsmFault::CommandError); return;
    }
    if (strcmp(line_, "OK")) return;
    if (state_ == GsmState::Submitting) {
        if (!sawCmgs_) { fail(now, GsmFault::UnexpectedOk); return; }
        fault_ = GsmFault::None;
        result_ = GsmResult::AcceptedByModem; state_ = GsmState::Ready;
        lastCheck_ = now; return;
    }
    if (state_ != GsmState::Initializing) return;
    if (step_ == 3 && !simReady_) { fail(now, GsmFault::SimNotReady); return; }
    if (step_ == 5 && !registered_) {
        fail(now, GsmFault::NotRegistered); return;
    }
    if (step_ == 6 && !smsSupported_) { fail(now, GsmFault::SmsUnsupported); return; }
    if (step_ == 7 && !serviceCentre_) { fail(now, GsmFault::ServiceCentreMissing); return; }
    if (polling_ && step_ == 7) {
        polling_ = false; state_ = GsmState::Ready; lastCheck_ = now; return;
    }
    const size_t count = sizeof(commands) / sizeof(commands[0]);
    if (++step_ >= count) {
        state_ = GsmState::Ready; lastCheck_ = now;
        fault_ = GsmFault::None;
    } else startCommand(now);
}
void GsmEngine::update(uint32_t now) {
    if (state_ == GsmState::Disabled) return;
    // Budget borne par tour : pas de boucle attendant le reseau ou un prompt.
    for (size_t n = 0; n < 192 && io_.available(); ++n) {
        const int input = io_.read(); if (input < 0) break;
        const char c = static_cast<char>(input);
        if (state_ == GsmState::Backoff) continue; // reponses tardives ignorees
        if (c == '>' && state_ == GsmState::Prompt) {
            const char ctrlZ = 26;
            state_ = GsmState::Submitting; since_ = now; used_ = 0;
            if (!io_.write(payload_, strlen(payload_)) || !io_.write(&ctrlZ, 1)) fail(now, GsmFault::WriteError);
        } else if (c == '\n') { if (used_) line(now); }
        else if (c != '\r') {
            if (used_ == 0 && c == ' ' && state_ == GsmState::Submitting) continue;
            if (used_ + 1 >= sizeof(line_)) { fail(now, GsmFault::LineTooLong); break; }
            line_[used_++] = c;
        }
    }
    if (state_ == GsmState::Backoff && now - since_ >= retryInterval_) {
        step_ = 0; startCommand(now);
    } else if ((state_ == GsmState::Initializing || state_ == GsmState::Prompt) &&
               now - since_ >= commandTimeout_) fail(now, GsmFault::PromptTimeout);
    else if (state_ == GsmState::Submitting && now - since_ >= submitTimeout_) fail(now, GsmFault::SubmitTimeout);
    else if (state_ == GsmState::Ready && (!registered_ || now - lastCheck_ >= retryInterval_)) {
        step_ = 4; polling_ = true; registered_ = false; startCommand(now);
    }
}
bool GsmEngine::submit(const char* receiver, const char* sms, uint32_t now) {
    size_t septets = 0;
    if (!ready() || !registered_ || !smsConfigured() ||
        result_ != GsmResult::None || !validE164(receiver) ||
        countD2Septets(sms, septets) != D2Error::Ok || septets == 0) return false;
    char command[40]; snprintf(command, sizeof(command), "AT+CMGS=\"%s\"\r", receiver);
    strcpy(payload_, sms); sawCmgs_ = false; used_ = 0;
    state_ = GsmState::Prompt; since_ = now;
    if (!io_.write(command, strlen(command))) fail(now, GsmFault::WriteError);
    // Meme echec immediat passe par takeResult : le service connait l'index actif.
    return true;
}
GsmResult GsmEngine::takeResult() { const GsmResult result = result_; result_ = GsmResult::None; return result; }
}
