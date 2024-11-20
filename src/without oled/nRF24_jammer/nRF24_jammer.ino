#include "SPI.h"
#include "EEPROM.h"
#include "RF24.h"
#include "html.h"
#include "WiFi.h"
#include "WebServer.h"
#include "options.h"
#include "jam.h"

void handleRoot() {
    server.send(200, "text/html", html);
}

void jamHandler(const char* htmlResponse, void (*jamFunction)()) {
    server.send(200, "text/html", htmlResponse);
    jamFunction();
}

void bluetooth_jammer() {
    jamHandler(html_bluetooth_jam, bluetooth_jam);
}
void drone_jammer() {
    jamHandler(html_drone_jam, drone_jam);
}
void wifi_jammer() {
    jamHandler(html_wifi_jam, wifi_jam);
}
void ble_jammer() {
    jamHandler(html_ble_jam, ble_jam);
}
void zigbee_jammer() {
    jamHandler(html_zigbee_jam, zigbee_jam);
}
void misc_jammer() {
    server.send(200, "text/html", html_misc_jammer);
}

void misc_channels() {
    int channel1 = server.arg("start").toInt();
    int channel2 = server.arg("stop").toInt();
    server.send(200, "text/html", html_misc_jam);
    misc_jam(channel1, channel2);
}

void settingsHandler(const char* htmlResponse) {
    server.send(200, "text/html", htmlResponse);
}

void setting_bluetooth_jam() { settingsHandler(html_bluetooth_setings); }
void setting_drone_jam() { settingsHandler(html_drone_setings); }
void setting_separate_together() { settingsHandler(html_separate_or_together); }
void setting_misc_jam() { settingsHandler(html_misc_setings); }

void storeEEPROM(int index, int value) {
    EEPROM.write(index, value);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void bluetooth_method_0() { storeEEPROM(0, 0); bluetooth_jam_method = 0; }
void bluetooth_method_1() { storeEEPROM(0, 1); bluetooth_jam_method = 1; }
void bluetooth_method_2() { storeEEPROM(0, 2); bluetooth_jam_method = 2; }
void drone_method_0() { storeEEPROM(1, 0); drone_jam_method = 0;}
void drone_method_1() { storeEEPROM(1, 1); drone_jam_method = 1;}
void separate_or_together_method_0() { storeEEPROM(4, 0); Separate_or_together = 0;}
void separate_or_together_method_1() { storeEEPROM(4, 1); Separate_or_together = 1;}
void misc_method_0(){ storeEEPROM(6, 0); misc_jam_method = 0;}
void misc_method_1(){ storeEEPROM(6, 1); misc_jam_method = 1;}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < 7; ++i) {
        if (EEPROM.read(i) == 255) {
            EEPROM.write(i, 0);
            EEPROM.commit();
        }
    }
    bluetooth_jam_method = EEPROM.read(0);
    drone_jam_method = EEPROM.read(1);
    ble_jam_method = EEPROM.read(2);
    wifi_jam_method = EEPROM.read(3);
    Separate_or_together = EEPROM.read(4);
    zigbee_jam_method = EEPROM.read(5);
    misc_jam_method = EEPROM.read(6);
    WiFi.softAP(ssid, password);
    server.on("/", handleRoot);
    server.on("/bluetooth_jam", bluetooth_jammer);
    server.on("/drone_jam", drone_jammer);
    server.on("/wifi_jam", wifi_jammer);
    server.on("/ble_jam", ble_jammer);
    server.on("/zigbee_jam", zigbee_jammer);
    server.on("/setting_bluetooth_jam", setting_bluetooth_jam);
    server.on("/setting_drone_jam", setting_drone_jam);
    server.on("/setting_separate_together", setting_separate_together);
    server.on("/bluetooth_method_0", bluetooth_method_0);
    server.on("/bluetooth_method_1", bluetooth_method_1);
    server.on("/bluetooth_method_2", bluetooth_method_2);
    server.on("/drone_method_0", drone_method_0);
    server.on("/drone_method_1", drone_method_1);
    server.on("/separate_or_together_method_0", separate_or_together_method_0);
    server.on("/separate_or_together_method_1", separate_or_together_method_1);
    server.on("/misc_jammer", misc_jammer);
    server.on("/misc_jam", misc_channels);
    server.on("/setting_misc_jam", setting_misc_jam);
    server.on("/misc_method_0", misc_method_0);
    server.on("/misc_method_1", misc_method_1);

    server.begin();
}

void loop() {
    server.handleClient();
}