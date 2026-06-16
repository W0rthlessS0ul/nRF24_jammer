#ifndef CONFIG_H
#define CONFIG_H

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "DNSServer.h"
#include "GyverButton.h"
#include "NimBLEDevice.h"
#include "Preferences.h"
#include "RF24.h"
#include "Update.h"
#include "WebServer.h"
#include "WiFi.h"
#include "esp_random.h"
#include "esp_wifi.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define default_ssid "jammer"
#define default_password "W0rthlessS0ul"
#define Version_Number "V3.3.0"
#define Version_Name "Flexible"

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

inline byte       bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
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

inline bool webServerStarted = false;
inline bool dnsServerStarted = false;

inline char   SSIDs_Array[100][33];
inline String APs_array[14];
inline int    WiFiScanChannels[14];
inline int    NumberChannels;
inline char   default_SSIDs_Array[100][33] = {
    "0 Never gonna give you up",
    "1 Never gonna let you down",
    "2 Never gonna run around",
    "3 And desert you",
    "4 Never gonna make you cry",
    "5 Never gonna say goodbye",
    "6 Never gonna tell a lie",
    "7 And hurt you",
};

inline int  channel1 = 0;
inline int  channel2 = 0;
inline int  flag     = 0;
inline bool hspi     = false;

inline SPIClass        *hp = nullptr;
inline GButton          btnOK(25);
inline GButton          btnNext(26);
inline GButton          btnPrevious(27);
inline WebServer        server(80);
inline DNSServer        dnsServer;
inline Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
inline Preferences      prefs;

#endif