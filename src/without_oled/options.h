#ifndef OPTIONS_H
#define OPTIONS_H

#include "Arduino.h"
#include "Update.h"
#include "EEPROM.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "GyverButton.h"
#include "WiFi.h"
#include "WebServer.h"
#include "RF24.h"

extern RF24 radio;
extern RF24 radio1;
extern const char *default_ssid;
extern const char *default_password;

extern size_t EEPROM_SIZE;
extern SPIClass *sp;
extern SPIClass *hp;
extern int bluetooth_jam_method;
extern int drone_jam_method;
extern int ble_jam_method;
extern int wifi_jam_method;
extern int zigbee_jam_method;
extern int Separate_or_together;
extern int misc_jam_method;
extern byte bluetooth_channels[];
extern byte bluetooth_channels_reversed[];
extern byte ble_channels[];
extern byte ble_channels_reversed[];
extern const char jam_text[17];
extern WebServer server;

#endif