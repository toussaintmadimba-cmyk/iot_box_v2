#ifndef PINS_H
#define PINS_H

// =====================================================
// IOT BOX - BROCHES ESP32
// =====================================================

// ---------- INA228 + INA3221 / I2C commun ----------
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// ---------- GPS NEO-8M / UART2 ----------

#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

// ---------- RTC DS1302 / THREE-WIRE ----------

#define DS1302_DATA_PIN 26
#define DS1302_CLOCK_PIN 25
#define DS1302_CE_PIN 27

// PZEM-004T 10 A : UART logiciel, distinct de UART0/1/2.
#ifndef PZEM_RX_PIN
#define PZEM_RX_PIN 35
#endif
#ifndef PZEM_TX_PIN
#define PZEM_TX_PIN 14
#endif

// ---------- INDICATION LOCALE DJUA ----------
#define LED_SYSTEM_PIN 23
#define LED_COMM_PIN 19
#define LED_ALERT_PIN 18
#define BUZZER_PIN 33

// ---------- A7670 : UART1 RX32 / TX13 (niveaux de carte a verifier) ----------
// Brochage cible : TXD modem -> RX32, RXD modem -> TX13, masse commune.
// Verifier les niveaux de la carte A7670.
// UART2 GPIO16/17 reste au GPS.
// Aucun PWRKEY n'est suppose par le firmware.
// VCC - GND - 13 - 32
 
#ifndef GSM_RX_PIN
#define GSM_RX_PIN 32
#endif
#ifndef GSM_TX_PIN
#define GSM_TX_PIN 13
#endif
// -1 : carte a demarrage automatique/manuellement mise sous tension.
// Sinon piloter PWRKEY par transistor, apres validation du schema de la carte.
#ifndef GSM_PWRKEY_PIN
#define GSM_PWRKEY_PIN -1
#endif

// ---------- FUTURE MICROSD ----------
// #define SD_CS_PIN 

#endif
