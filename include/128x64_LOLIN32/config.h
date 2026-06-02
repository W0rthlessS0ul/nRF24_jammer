#ifndef OPTIONS_H
#define OPTIONS_H

// ============================================================
// config.h - ESP32 WeMos LOLIN32 Lite + nRF24L01+PA+LNA
// ============================================================
// PINOUT:
//   nRF24L01+PA+LNA (VSPI):
//     VCC  -> 3.3V  (+ condensatore 100µF in parallelo!)
//     GND  -> GND
//     CE   -> GPIO 4
//     CSN  -> GPIO 5
//     SCK  -> GPIO 18
//     MOSI -> GPIO 23
//     MISO -> GPIO 19
//     IRQ  -> non collegato
//
//   Display OLED SSD1306 (I2C):
//     SDA  -> GPIO 21
//     SCL  -> GPIO 22
//
//   LED built-in LOLIN32 Lite:
//     GPIO 22  ACTIVE LOW (LOW=acceso, HIGH=spento)
//     Condivide con SCL ma I2C e open-drain, non conflitto.
//
//   Pulsanti (LOW attivo):
//     btnOK       -> GPIO 34  INPUT-ONLY: pull-up esterno 10k a 3.3V OBBLIGATORIO
//     btnNext     -> GPIO 35  INPUT-ONLY: pull-up esterno 10k a 3.3V OBBLIGATORIO
//     btnPrevious -> GPIO 32  (pull-up interno disponibile)
// ============================================================

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "DNSServer.h"
#include "EEPROM.h"
#include "GyverButton.h"
#include "NimBLEDevice.h"
#include "RF24.h"
#include "Update.h"
#include "WebServer.h"
#include "WiFi.h"
#include "esp_wifi.h"
#include "SPI.h"
#include "Wire.h"

// ---- Display ----
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_SDA      21
#define OLED_SCL      22

// ---- EEPROM ----
#define EEPROM_SIZE 512

// ---- WiFi AP defaults ----
#define default_ssid     "jammer"
#define default_password "W0rthlessS0ul"
#define Version_Number   "V3.2.0-LOLIN32"
#define Version_Name     "Flexible"

// ---- LED LOLIN32 Lite: GPIO 22, ACTIVE LOW ----
#define LED_PIN 22
#define LED_ON  LOW
#define LED_OFF HIGH

// ---- nRF24L01 pin (VSPI) ----
#define NRF_SCK         18
#define NRF_MOSI        23
#define NRF_MISO        19
#define DEFAULT_CE_PIN   4
#define DEFAULT_CSN_PIN  5

// ---- Pulsanti ----
#define BTN_OK_PIN   34
#define BTN_NEXT_PIN 35
#define BTN_PREV_PIN 32

// ============================================================
// Variabili globali inline
// ============================================================
inline int menu_number;
inline int display_setting;
inline int logo;
inline int access_point;
inline int buttons;
inline int Separate_or_together;

inline int bluetooth_jam_method;
inline int drone_jam_method;
inline int wifi_jam_method;
inline int misc_jam_method;
inline int nrf_pa;

inline byte       bluetooth_channels[] = {32,34,46,48,50,52,0,1,2,4,6,8,22,24,26,28,30,74,76,78,80};
inline byte       ble_channels[]       = {2, 26, 80};
inline const char jam_text[]           = "xxxxxxxxxxxxxxxx";

inline String BLE_MAC[100];
inline int    BLE_RSSI[100];
inline String BLE_Name[100];
inline String BLE_ManufacturerData[100];
inline String BLE_Payload[100];

inline int   nrf24_count;
inline int   ce_pins[30];
inline int   csn_pins[30];
inline RF24 *radios[30];

inline String APs_array[14];
inline int    WiFiScanChannels[14];
inline int    NumberChannels;

inline int  channel1 = 0;
inline int  channel2 = 0;
inline int  flag     = 0;
inline bool hspi     = false;

// VSPI: bus SPI hardware default su ESP32/LOLIN32 Lite
inline SPIClass *hp = nullptr;

// GButton LOW: il pulsante porta il pin a GND (pull-up esterno su 34/35)
inline GButton btnOK(BTN_OK_PIN, LOW);
inline GButton btnNext(BTN_NEXT_PIN, LOW);
inline GButton btnPrevious(BTN_PREV_PIN, LOW);

inline WebServer        server(80);
inline DNSServer        dnsServer;
inline Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#endif // OPTIONS_H
