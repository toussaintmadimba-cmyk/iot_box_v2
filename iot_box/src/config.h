#ifndef CONFIG_H
#define CONFIG_H

// IOT BOX - CONFIGURATION GENERALE
//  IDENTIFICATION BACKEND 
#define DEVICE_ID "DJUA-KIN-000001"

//  MONITEUR SERIE 
#define SERIAL_BAUD_RATE 115200

// Un seul transport actif. Surcharge possible a la compilation avec -D.
#define DJUA_PROFILE_SMS_PRODUCTION 1
#define DJUA_PROFILE_MQTT_TEST 2
#define DJUA_PROFILE_HTTP_TEST 3
#ifndef DJUA_COMM_PROFILE
#define DJUA_COMM_PROFILE DJUA_PROFILE_SMS_PRODUCTION
#endif
#if DJUA_COMM_PROFILE < 1 || DJUA_COMM_PROFILE > 3
#error "DJUA_COMM_PROFILE doit etre SMS_PRODUCTION, MQTT_TEST ou HTTP_TEST"
#endif

#define D2_REFERENCE_SHA "c5835077b2b6e8418e34898013cacc0892c12e25"
#define ELECTRICAL_SAMPLE_INTERVAL_MS 10000UL
#define D2_ELECTRICAL_MAX_AGE_MS 20000UL
#define D2_ENERGY_MAX_GAP_MS 20000UL
#ifndef D2_PERIOD_SECONDS
#define D2_PERIOD_SECONDS 30UL
#endif
static_assert(D2_PERIOD_SECONDS >= 10UL && D2_PERIOD_SECONDS <= 86400UL,
              "La periode D2 doit rester entre 10 et 86400 secondes");

// SIM800C sur UART1 (9600 bauds, RX32 / TX13) ; alimentation 2A et masse commune requises.
#ifndef GSM_HARDWARE_CONFIGURED
#define GSM_HARDWARE_CONFIGURED 1
#endif
#ifndef GSM_BAUD_RATE
#define GSM_BAUD_RATE 9600UL
#endif
#ifndef GSM_MODULE_LABEL
#define GSM_MODULE_LABEL "SIM800C"
#endif
#ifndef GSM_DEBUG_AT
#define GSM_DEBUG_AT 1
#endif
// PWRKEY optionnel via transistor : niveau actif cote ESP32, pas cote module.
#ifndef GSM_PWRKEY_ACTIVE_LEVEL
#define GSM_PWRKEY_ACTIVE_LEVEL 0
#endif
#define GSM_POWER_STABILIZE_MS 1000UL
#define GSM_PWRKEY_PULSE_MS 1200UL
#define GSM_BOOT_WAIT_MS 5000UL
#define GSM_COMMAND_TIMEOUT_MS 10000UL
#define GSM_SUBMIT_TIMEOUT_MS 35000UL
#define GSM_RETRY_INTERVAL_MS 15000UL
#define SMS_RETRY_MAX_MS 60000UL
static_assert(GSM_COMMAND_TIMEOUT_MS > 0 && GSM_SUBMIT_TIMEOUT_MS > 0 &&
              GSM_RETRY_INTERVAL_MS > 0 && SMS_RETRY_MAX_MS >= GSM_RETRY_INTERVAL_MS,
              "Les temporisations GSM/SMS doivent etre positives et coherentes");
// Active seulement pendant un provisionnement local supervise par USB.
#ifndef D2_PROVISIONING_ENABLED
#define D2_PROVISIONING_ENABLED 0
#endif
// Valeurs par defaut injectees une seule fois en NVS si le boitier est vierge (D2_UNPROVISIONED).
#ifndef D2_DEFAULT_HMAC_KEY_HEX
#define D2_DEFAULT_HMAC_KEY_HEX "ebe401b50ae09171a9fbc37231f79224efa195926ab90dc6dcc5e93a44f21f27"
#endif
#ifndef D2_DEFAULT_RECEIVER
#define D2_DEFAULT_RECEIVER "+243852869410"
#endif

// Convention D2 : positif = charge, negatif = decharge. A verifier au banc.
#ifndef BATTERY_CURRENT_POLARITY
#define BATTERY_CURRENT_POLARITY 1
#endif
static_assert(BATTERY_CURRENT_POLARITY == 1 || BATTERY_CURRENT_POLARITY == -1,
              "La polarite batterie doit etre 1 ou -1");

// Batterie nominale 12 V : limites de validite, independantes du pilote INA228.
#define BATTERY_MIN_VALID_V 9.0F
#define BATTERY_MAX_VALID_V 16.5F
#ifndef SOLAR_CURRENT_POLARITY
#define SOLAR_CURRENT_POLARITY 1
#endif

// GPS NEO-8M sur UART2, independant du modem.
#define GPS_BAUD_RATE 9600
#define GPS_MAX_AGE_MS 5000UL

// V2 sensor configuration. Enable after checking the physical board, straps,
// wiring, current limits and shunt rating. Defaults never assert commissioning.
#define V2_I2C_CLOCK_HZ 100000UL
#ifndef V2_BATTERY_CONFIGURED
#define V2_BATTERY_CONFIGURED 1
#endif
#ifndef V2_SOLAR_CONFIGURED
#define V2_SOLAR_CONFIGURED 1
#endif
#ifndef V2_DC_CONFIGURED
#define V2_DC_CONFIGURED 1
#endif
#ifndef V2_TV_CONFIGURED
#define V2_TV_CONFIGURED 1
#endif
#ifndef PZEM_HARDWARE_CONFIGURED
#define PZEM_HARDWARE_CONFIGURED 1
#endif
#ifndef D3_RECEIVER_ENABLED
#define D3_RECEIVER_ENABLED 1
#endif
#ifndef V2_BATTERY_ADDRESS
#define V2_BATTERY_ADDRESS 0x44
#endif
#ifndef V2_SOLAR_ADDRESS
#define V2_SOLAR_ADDRESS 0x41
#endif
#ifndef V2_DC_ADDRESS
#define V2_DC_ADDRESS 0x40
#endif
#ifndef V2_TV_ADDRESS
#define V2_TV_ADDRESS 0x45
#endif
// 30 A / 75 mV = 2.5 mOhm. Three independent physical resistances.
#ifndef BATTERY_SHUNT_OHMS
#define BATTERY_SHUNT_OHMS 0.0025
#endif
#ifndef SOLAR_SHUNT_OHMS
#define SOLAR_SHUNT_OHMS 0.0025
#endif
#ifndef TV_SHUNT_OHMS
#define TV_SHUNT_OHMS 0.015
#endif
#ifndef BATTERY_MAX_CURRENT_A
#define BATTERY_MAX_CURRENT_A 30.0
#endif
#ifndef SOLAR_MAX_CURRENT_A
#define SOLAR_MAX_CURRENT_A 30.0
#endif
#ifndef TV_MAX_CURRENT_A
#define TV_MAX_CURRENT_A 2.5
#endif
// 3 W is a required minimum assumed for configuration, not a measured rating.
#ifndef BATTERY_SHUNT_RATED_W
#define BATTERY_SHUNT_RATED_W 3.0
#endif
#ifndef SOLAR_SHUNT_RATED_W
#define SOLAR_SHUNT_RATED_W 3.0
#endif
#ifndef TV_SHUNT_RATED_W
#define TV_SHUNT_RATED_W 3.0
#endif
#ifndef BATTERY_ADC_NARROW_RANGE
#define BATTERY_ADC_NARROW_RANGE 0
#endif
#ifndef SOLAR_ADC_NARROW_RANGE
#define SOLAR_ADC_NARROW_RANGE 0
#endif
#ifndef TV_ADC_NARROW_RANGE
#define TV_ADC_NARROW_RANGE 0
#endif
#ifndef SOLAR_MAX_BUS_V
#define SOLAR_MAX_BUS_V 85.0
#endif
#ifndef TV_MAX_BUS_V
#define TV_MAX_BUS_V 26.0
#endif
#ifndef TV_CURRENT_POLARITY
#define TV_CURRENT_POLARITY 1
#endif
#ifndef DC1_SHUNT_OHMS
#define DC1_SHUNT_OHMS 0.020
#endif
#ifndef DC2_SHUNT_OHMS
#define DC2_SHUNT_OHMS 0.020
#endif
#ifndef DC3_SHUNT_OHMS
#define DC3_SHUNT_OHMS 0.020
#endif
// Provisional 5 A limits and 1 W minimum rating; verify each physical port.
#ifndef DC1_MAX_CURRENT_A
#define DC1_MAX_CURRENT_A 5.0
#endif
#ifndef DC2_MAX_CURRENT_A
#define DC2_MAX_CURRENT_A 5.0
#endif
#ifndef DC3_MAX_CURRENT_A
#define DC3_MAX_CURRENT_A 5.0
#endif
#ifndef DC1_SHUNT_RATED_W
#define DC1_SHUNT_RATED_W 1.0
#endif
#ifndef DC2_SHUNT_RATED_W
#define DC2_SHUNT_RATED_W 1.0
#endif
#ifndef DC3_SHUNT_RATED_W
#define DC3_SHUNT_RATED_W 1.0
#endif
#ifndef DC1_CURRENT_POLARITY
#define DC1_CURRENT_POLARITY 1
#endif
#ifndef DC2_CURRENT_POLARITY
#define DC2_CURRENT_POLARITY 1
#endif
#ifndef DC3_CURRENT_POLARITY
#define DC3_CURRENT_POLARITY 1
#endif
#define PZEM_BAUD_RATE 9600UL
#ifndef PZEM_MODBUS_ADDRESS
#define PZEM_MODBUS_ADDRESS 0xF8
#endif
#define PZEM_POLL_INTERVAL_MS 2000UL
#define PZEM_MAX_AGE_MS 5000UL
#define PZEM_REPLY_TIMEOUT_MS 1000UL
static_assert(V2_BATTERY_ADDRESS != V2_SOLAR_ADDRESS && V2_BATTERY_ADDRESS != V2_TV_ADDRESS &&
    V2_BATTERY_ADDRESS != V2_DC_ADDRESS && V2_SOLAR_ADDRESS != V2_TV_ADDRESS &&
    V2_SOLAR_ADDRESS != V2_DC_ADDRESS && V2_TV_ADDRESS != V2_DC_ADDRESS, "Conflit I2C V2");
static_assert(BATTERY_MAX_CURRENT_A <= 30 && SOLAR_MAX_CURRENT_A <= 30 && TV_MAX_CURRENT_A <= 30 &&
    DC1_MAX_CURRENT_A <= 30 && DC2_MAX_CURRENT_A <= 30 && DC3_MAX_CURRENT_A <= 30,
    "Courants V2 hors du contrat D3");

//  GEOFENCING LOCAL
#define GEOFENCE_ENABLED 1
#define GEOFENCE_CENTER_LAT (-4.3308595)
#define GEOFENCE_CENTER_LON (15.30678333)
#define GEOFENCE_ENTER_RADIUS_M 10.0F
#define GEOFENCE_EXIT_RADIUS_M 15.0F
#define GEOFENCE_CHECK_INTERVAL_MS 5000UL
#define GEOFENCE_CONFIRM_COUNT 3U

//  HORLOGE RTC DS1302
// Le DS1302 conserve l'heure locale correspondant a ce fuseau fixe.
#define RTC_GMT_OFFSET_MINUTES 60
#define RTC_TIMEZONE_LABEL "GMT+1"
#ifndef RTC_AUTO_INITIALIZE_IF_INVALID
// Production : ne pas presenter l'heure de compilation comme une heure fiable.
#define RTC_AUTO_INITIALIZE_IF_INVALID (DJUA_COMM_PROFILE != DJUA_PROFILE_SMS_PRODUCTION)
#endif

// Wi-Fi
#define WIFI_SSID "ODC_LOCAL"
// Two literal backslashes in the password are represented by four in this C++ string.
#define WIFI_PASSWORD "SecureTopTop=ABC_Puits"

//BACKEND REEL
#define BACKEND_URL "http://10.252.252.40:5000/api/iot/telemetry"
#define IOT_API_KEY "djua"
#define ENABLE_HTTP_BACKEND (DJUA_COMM_PROFILE == DJUA_PROFILE_HTTP_TEST)

// MQTT TESTING
// Public broker for short-term integration tests only. Do not use this broker
// or these public topics for production or sensitive telemetry.
#define MQTT_BROKER_HOST "test.mosquitto.org"
#define MQTT_BROKER_PORT 1883
#define MQTT_TOPIC_PREFIX "djua/test"
#define MQTT_RETRY_INTERVAL_MS 10000UL

#define TELEMETRY_INTERVAL_MS 10000UL
#define WIFI_RETRY_INTERVAL_MS 10000UL

// Indication locale : LEDs cablees vers GND avec resistance serie.
#define LED_ACTIVE_HIGH 1
#define SYSTEM_BOOT_BLINK_MS 200UL
#define SYSTEM_HEARTBEAT_PERIOD_MS 2000UL
#define SYSTEM_HEARTBEAT_ON_MS 100UL
#define COMM_SLOW_BLINK_MS 500UL
#define COMM_FAST_BLINK_MS 150UL
#define ALERT_WARNING_BLINK_MS 500UL
#define ALERT_CRITICAL_BLINK_MS 150UL

// Surcharges possibles pour le sketch materiel et les compilations de test.
#ifndef BUZZER_ENABLED
#define BUZZER_ENABLED 1
#endif
#ifndef BUZZER_ACTIVE_HIGH
#define BUZZER_ACTIVE_HIGH 1
#endif
#ifndef BUZZER_PASSIVE
#define BUZZER_PASSIVE 0
#endif
#define BUZZER_FREQUENCY_HZ 2000UL
#define BUZZER_SHORT_BEEP_MS 100UL
#define BUZZER_LONG_BEEP_MS 700UL
#define BUZZER_GAP_MS 150UL
#define BUZZER_GEOFENCE_GAP_MS 200UL
#define BUZZER_GEOFENCE_SHORT_MS 150UL
#define BUZZER_SERVICE_PERIOD_MS 10UL

// Compteurs de cycles de mesure, apres la recuperation propre aux capteurs.
#define SENSOR_WARNING_FAILURE_COUNT 3U
#define BATTERY_SENSOR_CRITICAL_FAILURE_COUNT 3U
#define TELEMETRY_WARNING_FAILURE_COUNT 3U
#define MQTT_WARNING_GRACE_MS 30000UL
#define HTTP_SUCCESS_VALID_MS (3UL * TELEMETRY_INTERVAL_MS)
#define WIFI_CONNECT_TIMEOUT_MS 15000UL

// Aucun seuil metier batterie n'est suppose. Pour activer, definir les deux
// tensions selon la technologie et les exigences de la batterie installee.
#define BATTERY_ALERTS_ENABLED 0
// #define BATTERY_WARNING_V ...
// #define BATTERY_CRITICAL_V ...
#if BATTERY_ALERTS_ENABLED
#if !defined(BATTERY_WARNING_V) || !defined(BATTERY_CRITICAL_V)
#error "Configurer BATTERY_WARNING_V et BATTERY_CRITICAL_V avant activation"
#endif
#endif

#endif
