#ifndef OPTIONS_H
#define OPTIONS_H

#include "Update.h"
#include "EEPROM.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "GyverButton.h"
#include "WiFi.h"
#include "DNSServer.h"
#include "WebServer.h"
#include "RF24.h"

extern GButton butt1;
extern GButton buttNext;
extern GButton buttPrevious;
extern const char *default_ssid;
extern const char *default_password;
extern String Version_Number;
extern String Version_Name;

extern uint8_t SCREEN_WIDTH;
extern uint8_t SCREEN_HEIGHT;
extern int8_t OLED_RESET;
extern size_t EEPROM_SIZE;
extern Adafruit_SSD1306 display;
extern SPIClass *hp;
extern int menu_number;
extern int bluetooth_jam_method;
extern int drone_jam_method;
extern int display_setting;
extern int wifi_jam_method;
extern int nrf_pa;
extern int Separate_or_together;
extern int misc_jam_method;
extern int logo;
extern int access_point;
extern int buttons;
extern int channel1;
extern int channel2;
extern int flag;
extern bool hspi;
extern byte bluetooth_channels[];
extern byte ble_channels[];
extern const char jam_text[17];
extern RF24 *radios[30];
extern int ce_pins[30];
extern int csn_pins[30];
extern int WiFiScanChannels[14];
extern int nrf24_count;
extern WebServer server;
extern DNSServer dnsServer;

#endif