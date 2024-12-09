#include "SPI.h"
#include "Update.h"
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

void sendHtmlAndExecute(const char* htmlResponse, void (*action)() = nullptr) {
    server.send(200, "text/html", htmlResponse);
    if (action) action();
}

void jamHandler(const char* htmlResponse, void (*jamFunction)()) {
    sendHtmlAndExecute(htmlResponse, jamFunction);
}

void miscChannelsHandler() {
    int channel1 = server.arg("start").toInt();
    int channel2 = server.arg("stop").toInt();
    sendHtmlAndExecute(html_misc_jam);
    misc_jam(channel1, channel2);
}

void wifiChannelsHandler() {
    int channel = server.arg("channel").toInt();
    sendHtmlAndExecute(html_wifi_jam);
    wifi_channel(channel);
}

void settingsHandler(const char* htmlResponse) {
    sendHtmlAndExecute(htmlResponse);
}

void storeEEPROMAndSet(int index, int value, int& targetVar) {
    EEPROM.write(index, value);
    EEPROM.commit();
    targetVar = value;
    sendHtmlAndExecute(html);
}

void registerRoute(const char* path, void (*handler)()) {
    server.on(path, handler);
}

void handleFileUpload() {
    HTTPUpload &upload = server.upload();
    static unsigned long lastUpdate = 0; 
    const unsigned long updateInterval = 100;

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
            server.send(200, "text/plain", "Update Failed");
            return;
        } else {
            server.sendContent(html_upload_progress);
            lastUpdate = millis();
        }
    } 
    else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }

        if (millis() - lastUpdate >= updateInterval) {
            lastUpdate = millis();
            int progress = (100.0 * Update.progress() / Update.size());
            server.sendContent(String("<script>updateProgress(") + progress + ");</script>");
        }
    } 
    else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("Update Success: %u bytes\n", upload.totalSize);
            server.sendContent(String("<script>updateProgress(100); document.getElementById('status').innerHTML = 'Update Success';</script>"));
            delay(2000);
            ESP.restart();
        } else {
            Update.printError(Serial);
            server.sendContent("<script>document.getElementById('status').innerHTML = 'Update Failed';</script>");
        }
    }
}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    // Initialize EEPROM values if unset
    for (int i = 0; i < 7; ++i) {
        if (EEPROM.read(i) == 255) {
            EEPROM.write(i, 0);
            EEPROM.commit();
        }
    }

    // Load settings
    bluetooth_jam_method = EEPROM.read(0);
    drone_jam_method = EEPROM.read(1);
    ble_jam_method = EEPROM.read(2);
    wifi_jam_method = EEPROM.read(3);
    Separate_or_together = EEPROM.read(4);
    zigbee_jam_method = EEPROM.read(5);
    misc_jam_method = EEPROM.read(6);

    WiFi.softAP(ssid, password);

    // Register routes
    registerRoute("/", handleRoot);
    registerRoute("/bluetooth_jam", []() { jamHandler(html_bluetooth_jam, bluetooth_jam); });
    registerRoute("/drone_jam", []() { jamHandler(html_drone_jam, drone_jam); });
    registerRoute("/wifi_jam", []() { jamHandler(html_wifi_jam, wifi_jam); });
    registerRoute("/ble_jam", []() { jamHandler(html_ble_jam, ble_jam); });
    registerRoute("/zigbee_jam", []() { jamHandler(html_zigbee_jam, zigbee_jam); });
    registerRoute("/misc_jammer", []() { sendHtmlAndExecute(html_misc_jammer); });
    registerRoute("/misc_jam", miscChannelsHandler);
    registerRoute("/wifi_selected_jam", wifiChannelsHandler);

    registerRoute("/setting_bluetooth_jam", []() { settingsHandler(html_bluetooth_setings); });
    registerRoute("/setting_drone_jam", []() { settingsHandler(html_drone_setings); });
    registerRoute("/setting_separate_together", []() { settingsHandler(html_separate_or_together); });
    registerRoute("/setting_misc_jam", []() { settingsHandler(html_misc_setings); });
    registerRoute("/OTA", []() { settingsHandler(html_ota); });
    registerRoute("/wifi_select", []() { settingsHandler(html_wifi_select); });
    registerRoute("/wifi_channel", []() { settingsHandler(html_wifi_channel); });
    
    server.on("/update", HTTP_POST, []() {
        server.send(200, "text/plain", (Update.end() ? "Update Success" : "Update Failed"));
    }, handleFileUpload);

    registerRoute("/bluetooth_method_0", []() { storeEEPROMAndSet(0, 0, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_1", []() { storeEEPROMAndSet(0, 1, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_2", []() { storeEEPROMAndSet(0, 2, bluetooth_jam_method); });
    registerRoute("/drone_method_0", []() { storeEEPROMAndSet(1, 0, drone_jam_method); });
    registerRoute("/drone_method_1", []() { storeEEPROMAndSet(1, 1, drone_jam_method); });
    registerRoute("/separate_or_together_method_0", []() { storeEEPROMAndSet(4, 0, Separate_or_together); });
    registerRoute("/separate_or_together_method_1", []() { storeEEPROMAndSet(4, 1, Separate_or_together); });
    registerRoute("/misc_method_0", []() { storeEEPROMAndSet(6, 0, misc_jam_method); });
    registerRoute("/misc_method_1", []() { storeEEPROMAndSet(6, 1, misc_jam_method); });

    server.begin();
}

void loop() {
    server.handleClient();
}