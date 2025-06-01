#include "Arduino.h"
#include "SPI.h"
#include "Update.h"
#include "EEPROM.h"
#include "RF24.h"
#include "html.h"
#include "WiFi.h"
#include "WebServer.h"
#include "options.h"
#include "jam.h"

// Serial command buffer
char serialBuffer[256];
int serialBufferPos = 0;

// Jamming state
enum JamMode {
  NONE,
  BLUETOOTH_JAM,
  DRONE_JAM,
  WIFI_JAM,
  BLE_JAM,
  ZIGBEE_JAM,
  MISC_JAM,
  WIFI_CHANNEL_JAM // Specific Wi-Fi channel jamming
};
JamMode currentJamMode = NONE;
bool isJammingActive = false;

// Forward declarations
void stop_jam();
void handleSerialCommand(String command);
// void bluetooth_jam_step(); // Already in jam.h
// void drone_jam_step();
// void wifi_jam_step();
// void ble_jam_step();
// void zigbee_jam_step();
// void misc_jam_step();
// void wifi_channel_jam_step();
void reset_jam_state_variables(); // Now declared in jam.h, ensure it's included properly

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

void initWiFiSettings() {
    if (EEPROM.read(10) == 255) {
        saveWiFiSettings(default_ssid, default_password);
    }
}

void saveWiFiSettings(const char* new_ssid, const char* new_password) {
    for (int i = 0; i < 32; i++) {
        if (i < strlen(new_ssid)) {
            EEPROM.write(10 + i, new_ssid[i]);
        } else {
            EEPROM.write(10 + i, 0);
        }
    }
    
    for (int i = 0; i < 32; i++) {
        if (i < strlen(new_password)) {
            EEPROM.write(42 + i, new_password[i]);
        } else {
            EEPROM.write(42 + i, 0);
        }
    }
    
    EEPROM.commit();
}

String getSSIDFromEEPROM() {
    char ssid[33] = {0};
    for (int i = 0; i < 32; i++) {
        ssid[i] = EEPROM.read(10 + i);
        if (ssid[i] == 0) break;
    }
    return String(ssid);
}

String getPasswordFromEEPROM() {
    char password[33] = {0};
    for (int i = 0; i < 32; i++) {
        password[i] = EEPROM.read(42 + i);
        if (password[i] == 0) break;
    }
    return String(password);
}

void handleSaveWiFiSettings() {
    String new_ssid = server.arg("ssid");
    String new_password = server.arg("password");

    saveWiFiSettings(new_ssid.c_str(), new_password.c_str());

    server.send(200, "text/html", html_pls_reboot);
    delay(1000);
    ESP.restart();
}

void handleResetWiFiSettings() {
    saveWiFiSettings(default_ssid, default_password);

    server.send(200, "text/html", html_pls_reboot);
    delay(1000);
    ESP.restart();
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
            delay(100);
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

    initWiFiSettings();

    // Load settings
    bluetooth_jam_method = EEPROM.read(0);
    drone_jam_method = EEPROM.read(1);
    ble_jam_method = EEPROM.read(2);
    wifi_jam_method = EEPROM.read(3);
    Separate_or_together = EEPROM.read(4);
    zigbee_jam_method = EEPROM.read(5);
    misc_jam_method = EEPROM.read(6);

    String current_ssid = getSSIDFromEEPROM();
    String current_password = getPasswordFromEEPROM();
        
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());

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
    registerRoute("/wifi_settings", []() { settingsHandler(html_wifi_settings); });
    
    server.on("/update", HTTP_POST, []() {
        server.send(200, "text/plain", (Update.end() ? "Update Success" : "Update Failed"));
    }, handleFileUpload);
    server.on("/save_wifi_settings", handleSaveWiFiSettings);
    server.on("/reset_wifi_settings", handleResetWiFiSettings);

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
    Serial.println("Serial command interface ready.");
}

void loop() {
    server.handleClient();

    // Read serial data
    while (Serial.available() > 0) {
        char incomingChar = Serial.read();
        if (incomingChar == '\n') {
            serialBuffer[serialBufferPos] = '\0'; // Null-terminate the string
            handleSerialCommand(String(serialBuffer));
            serialBufferPos = 0; // Reset buffer position
        } else if (serialBufferPos < sizeof(serialBuffer) - 1) {
            serialBuffer[serialBufferPos++] = incomingChar;
        }
    }

    if (isJammingActive) {
        switch (currentJamMode) {
            case BLUETOOTH_JAM:
                bluetooth_jam_step();
                break;
            case DRONE_JAM:
                drone_jam_step();
                break;
            case WIFI_JAM:
                wifi_jam_step();
                break;
            case BLE_JAM:
                ble_jam_step();
                break;
            case ZIGBEE_JAM:
                zigbee_jam_step();
                break;
            case MISC_JAM:
                misc_jam_step();
                break;
            case WIFI_CHANNEL_JAM:
                wifi_channel_jam_step();
                break;
            case NONE:
                // Nothing to do
                break;
        }
    }
}

void stop_jam() {
    isJammingActive = false;
    currentJamMode = NONE;
    radio.stopConstCarrier();
    radio1.stopConstCarrier();
    radio.powerDown();
    radio1.powerDown();
    reset_jam_state_variables(); // Reset indices and counters
    Serial.println("Jamming STOPPED. State variables reset.");
}

void handleSerialCommand(String command) {
    Serial.print("Received command: ");
    Serial.println(command);

    if (command.startsWith("START ")) {
        stop_jam(); // Stop any current jamming and reset states before starting new
        isJammingActive = true; // Set active true after potential stop
        String jamType = command.substring(6);
        reset_jam_state_variables(); // Reset counters for the new mode

        if (jamType == "BT") {
            currentJamMode = BLUETOOTH_JAM;
            HSPI_init(true); VSPI_init(true);
            Serial.println("Starting Bluetooth Jamming");
        } else if (jamType == "DRONE") {
            currentJamMode = DRONE_JAM;
            HSPI_init(true); VSPI_init(true);
            Serial.println("Starting Drone Jamming");
        } else if (jamType == "WIFI") {
            currentJamMode = WIFI_JAM;
            HSPI_init(false); VSPI_init(false);
            Serial.println("Starting WiFi Jamming");
        } else if (jamType == "BLE") {
            currentJamMode = BLE_JAM;
            HSPI_init(false); VSPI_init(false);
            Serial.println("Starting BLE Jamming");
        } else if (jamType == "ZIGBEE") {
            currentJamMode = ZIGBEE_JAM;
            HSPI_init(false); VSPI_init(false);
            Serial.println("Starting Zigbee Jamming");
        } else if (jamType == "MISC") {
            currentJamMode = MISC_JAM;
            if (misc_jam_method == 1) { // Payload
                HSPI_init(false); VSPI_init(false);
            } else { // Carrier
                HSPI_init(true); VSPI_init(true);
            }
            Serial.println("Starting Misc Jamming. Ensure MISC_CHANNELS are set.");
        } else if (jamType == "WIFI_CH") {
            currentJamMode = WIFI_CHANNEL_JAM;
            HSPI_init(false); VSPI_init(false);
            Serial.println("Starting WiFi Channel Jamming. Ensure WIFI_CHANNEL is set.");
        } else {
            isJammingActive = false;
            currentJamMode = NONE;
            Serial.println("Unknown START command type.");
        }
    } else if (command == "STOP") {
        stop_jam();
    } else if (command.startsWith("SET ")) {
        String setting = command.substring(4);
        if (setting.startsWith("BT_METHOD ")) {
            int method = setting.substring(10).toInt();
            if (method >= 0 && method <= 2) {
                EEPROM.write(0, method); EEPROM.commit();
                bluetooth_jam_method = method;
                Serial.print("Set Bluetooth Jam Method to: "); Serial.println(method);
            } else { Serial.println("Invalid Bluetooth method value."); }
        } else if (setting.startsWith("DRONE_METHOD ")) {
            int method = setting.substring(13).toInt();
            if (method >= 0 && method <= 1) {
                EEPROM.write(1, method); EEPROM.commit();
                drone_jam_method = method;
                Serial.print("Set Drone Jamming Method to: "); Serial.println(method);
            } else { Serial.println("Invalid Drone method value."); }
        } else if (setting.startsWith("BLE_METHOD ")) {
            int method = setting.substring(11).toInt();
            EEPROM.write(2, method); EEPROM.commit();
            ble_jam_method = method; // Assuming ble_jam_method is defined globally
            Serial.print("Set BLE Jamming Method to: "); Serial.println(method);
        } else if (setting.startsWith("WIFI_METHOD ")) {
            int method = setting.substring(12).toInt();
            EEPROM.write(3, method); EEPROM.commit();
            wifi_jam_method = method; // Assuming wifi_jam_method is defined globally
            Serial.print("Set WiFi Jamming Method to: "); Serial.println(method);
        } else if (setting.startsWith("SEP_OR_TOGETHER ")) {
            int value = setting.substring(16).toInt();
            if (value == 0 || value == 1) {
                EEPROM.write(4, value); EEPROM.commit();
                Separate_or_together = value;
                Serial.print("Set Separate_or_together to: "); Serial.println(value);
            } else { Serial.println("Invalid Separate_or_together value."); }
        } else if (setting.startsWith("ZIGBEE_METHOD ")) {
            int method = setting.substring(14).toInt();
            EEPROM.write(5, method); EEPROM.commit();
            zigbee_jam_method = method; // Assuming zigbee_jam_method is defined globally
            Serial.print("Set Zigbee Jamming Method to: "); Serial.println(method);
        } else if (setting.startsWith("MISC_METHOD ")) {
            int method = setting.substring(12).toInt();
             if (method == 0 || method == 1) {
                EEPROM.write(6, method); EEPROM.commit();
                misc_jam_method = method;
                Serial.print("Set Misc Jamming Method to: "); Serial.println(method);
            } else { Serial.println("Invalid Misc method value."); }
        } else if (setting.startsWith("MISC_CHANNELS ")) { // e.g., SET MISC_CHANNELS 1 10
            String params = setting.substring(14);
            int spaceIdx = params.indexOf(' ');
            if (spaceIdx != -1) {
                misc_jam_ch1 = params.substring(0, spaceIdx).toInt();
                misc_jam_ch2 = params.substring(spaceIdx + 1).toInt();
                if (misc_jam_ch1 > 0 && misc_jam_ch2 > 0 && misc_jam_ch1 <= misc_jam_ch2 && misc_jam_ch2 <= 125) { // Basic validation
                    Serial.print("Set Misc Jam Channels to: "); Serial.print(misc_jam_ch1); Serial.print("-"); Serial.println(misc_jam_ch2);
                } else {
                    Serial.println("Invalid channel values for MISC_CHANNELS. Must be 1-125, start <= end.");
                    misc_jam_ch1 = 1; misc_jam_ch2 = 80; // Reset to default
                }
            } else { Serial.println("Invalid format for MISC_CHANNELS. Use: SET MISC_CHANNELS <start> <end>");}
        } else if (setting.startsWith("WIFI_CHANNEL ")) { // e.g., SET WIFI_CHANNEL 6
            int channel = setting.substring(13).toInt();
            if (channel >= 1 && channel <= 13) { // Standard Wi-Fi channels
                wifi_selected_jam_channel = channel;
                Serial.print("Set WiFi Selected Jam Channel to: "); Serial.println(wifi_selected_jam_channel);
            } else { Serial.println("Invalid WiFi channel. Must be 1-13."); }
        }
        else {
            Serial.println("Unknown SET command parameter.");
        }
    } else if (command == "GET SETTINGS") {
        Serial.println("Current Settings:");
        Serial.print("Bluetooth Jam Method: "); Serial.println(bluetooth_jam_method);
        Serial.print("Drone Jam Method: "); Serial.println(drone_jam_method);
        Serial.print("BLE Jam Method: "); Serial.println(ble_jam_method); // Ensure ble_jam_method is globally accessible
        Serial.print("WiFi Jam Method: "); Serial.println(wifi_jam_method); // Ensure wifi_jam_method is globally accessible
        Serial.print("Separate or Together: "); Serial.println(Separate_or_together);
        Serial.print("Zigbee Jam Method: "); Serial.println(zigbee_jam_method); // Ensure zigbee_jam_method is globally accessible
        Serial.print("Misc Jam Method: "); Serial.println(misc_jam_method);
        Serial.print("Misc Jam Channels: "); Serial.print(misc_jam_ch1); Serial.print("-"); Serial.println(misc_jam_ch2);
        Serial.print("WiFi Selected Jam Channel: "); Serial.println(wifi_selected_jam_channel);
    } else {
        Serial.println("Unknown command.");
    }
}