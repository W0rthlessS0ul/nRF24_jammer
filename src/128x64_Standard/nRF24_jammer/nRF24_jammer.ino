#include "SPI.h"
#include "Update.h"
#include "EEPROM.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "GyverButton.h"
#include "RF24.h"
#include "bitmap.h"
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
    delay(500);
    if (action) action();
}

void jamHandler(const char* htmlResponse, void (*jamFunction)(), const unsigned char* bitmap) {
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
    display.display();
    sendHtmlAndExecute(htmlResponse, jamFunction);
    updateDisplay(menu_number);
}

void miscChannelsHandler() {
    int channel1 = server.arg("start").toInt();
    int channel2 = server.arg("stop").toInt();
    sendHtmlAndExecute(html_misc_jam);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Start");
    display.setCursor(96, 0);
    display.println("Stop");
    display.setCursor(52, 0);
    display.println("Width");
    display.setCursor(8, 10);
    display.println(String(channel1));
    display.setCursor(60, 10);
    display.println(String(channel2 - channel1));
    display.setCursor(100, 10);
    display.println(String(channel2));
    display.setCursor(0, 20);
    display.println("Jamming Started");
    display.display();
    misc_jam(channel1, channel2);
}

void wifiChannelsHandler() {
    int channel = server.arg("channel").toInt();
    sendHtmlAndExecute(html_wifi_jam);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
    display.display();
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

    if (new_ssid == "" && new_password == "") {
        server.send(200, "text/html", html_pls_reboot);
        storeEEPROMAndSet(8, 1, access_point);
        return;
    }

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
            display.clearDisplay();
            display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 64, WHITE);
            display.display();
            delay(1000);
            ESP.restart();
        } else {
            Update.printError(Serial);
            server.sendContent("<script>document.getElementById('status').innerHTML = 'Update Failed';</script>");
        }
    }
}

void misc() {
    auto display_info = [&](String info) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Start");
        display.setCursor(96, 0);
        display.println("Stop");
        display.setCursor(52, 0);
        display.println("Width");
        display.setCursor(8, 10);
        display.println(String(channel1));
        display.setCursor(60, 10);
        display.println(String(channel2 - channel1));
        display.setCursor(100, 10);
        display.println(String(channel2));
        display.setCursor(0, 20);
        display.println(info);
        display.display();
    };

    display_info("");

    auto incrementChannel = [&](int& channel) {
        channel++;
        if (channel > 125) {
            channel = 0;
        }
    };
    auto reduceChannel = [&](int& channel) {
        channel--;
        if (channel < 0) {
            channel = 125;
        }
    };

    while (true) {
        butt1.tick();
        buttNext.tick();
        buttPrevious.tick();
        if (buttons == 0){
            if (butt1.isSingle() || butt1.isHold()) {
                if (butt1.isHold()) {
                    delay(100);
                }
                incrementChannel(flag == 0 ? channel1 : channel2);
                display_info("");
            } 
            else if (butt1.isDouble()) {
                if (flag == 0) {
                    flag = 1;
                } 
                else {
                    if (channel1 > channel2) {
                        display_info("Error: Second < First");
                        flag = 0;
                    }
                    else {
                        display_info("Jamming Started");
                        misc_jam(channel1, channel2);
                    }
                }
            }
        }
        if (buttons == 1){
            if (buttNext.isSingle() || buttNext.isHold()) {
                if (buttNext.isHold()) {
                    delay(100);
                }
                incrementChannel(flag == 0 ? channel1 : channel2);
                display_info("");
            } 
            else if (butt1.isSingle()) {
                if (flag == 0) {
                    flag = 1;
                } 
                else {
                    if (channel1 > channel2) {
                        display_info("Error: Second < First");
                        flag = 0;
                    }
                    else {
                        display_info("Jamming Started");
                        misc_jam(channel1, channel2);
                    }
                }
            }
        }
        if (buttons == 2){
            if (buttNext.isSingle() || buttNext.isHold()) {
                if (buttNext.isHold()) {
                    delay(100);
                }
                incrementChannel(flag == 0 ? channel1 : channel2);
                display_info("");
            } 
            if (buttPrevious.isSingle() || buttPrevious.isHold()) {
                if (buttPrevious.isHold()) {
                    delay(100);
                }
                reduceChannel(flag == 0 ? channel1 : channel2);
                display_info("");
            } 
            else if (butt1.isSingle()) {
                if (flag == 0) {
                    flag = 1;
                } 
                else {
                    if (channel1 > channel2) {
                        display_info("Error: Second < First");
                        flag = 0;
                    }
                    else {
                        display_info("Jamming Started");
                        misc_jam(channel1, channel2);
                    }
                }
            }
        }
    }
}

void wifi_select(){
    auto display_info = [&](int flag) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("channel: "+String(flag));
        display.display();
    };
    while (true){
        butt1.tick();
        buttNext.tick();
        buttPrevious.tick();
        if (buttons == 0){
            if (butt1.isSingle()) {
                menu_number = (menu_number + 1) % 2;
                display.clearDisplay();
                const uint8_t* bitmap = (menu_number == 0) ? bitmap_wifi_all : bitmap_wifi_select;
                display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
                display.display();
            }
            if (butt1.isHolded()) {
                if (menu_number == 1){
                    flag = 0;
                    display_info(flag);
                    while (true){
                        butt1.tick();
                        if (butt1.isSingle()){
                            flag = flag + 1;
                            if (flag > 13){
                              flag = 0;
                            }
                            display_info(flag);
                        }
                        if (butt1.isHolded()){
                            display.clearDisplay();
                            display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
                            display.display();
                            wifi_channel(flag);
                        }
                    }
                }
                display.clearDisplay();
                display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
                display.display();
                wifi_jam();
            }
        }
        if (buttons == 1){
            if (buttNext.isSingle()) {
                menu_number = (menu_number + 1) % 2;
                display.clearDisplay();
                const uint8_t* bitmap = (menu_number == 0) ? bitmap_wifi_all : bitmap_wifi_select;
                display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
                display.display();
            }
            if (butt1.isSingle()) {
                if (menu_number == 1){
                    flag = 0;
                    display_info(flag);
                    while (true){
                        butt1.tick();
                        buttNext.tick();
                        if (buttNext.isSingle()){
                            flag = flag + 1;
                            if (flag > 13){
                              flag = 0;
                            }
                            display_info(flag);
                        }
                        if (butt1.isSingle()){
                            display.clearDisplay();
                            display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
                            display.display();
                            wifi_channel(flag);
                        }
                    }
                }
                display.clearDisplay();
                display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
                display.display();
                wifi_jam();
            }
        }
        if (buttons == 2){
            if (buttNext.isSingle()) {
                menu_number = (menu_number + 1) % 2;
                display.clearDisplay();
                const uint8_t* bitmap = (menu_number == 0) ? bitmap_wifi_all : bitmap_wifi_select;
                display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
                display.display();
            }
            if (buttPrevious.isSingle()) {
                menu_number = (menu_number + 1) % 2;
                display.clearDisplay();
                const uint8_t* bitmap = (menu_number == 0) ? bitmap_wifi_all : bitmap_wifi_select;
                display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
                display.display();
            }
    
            if (butt1.isSingle()) {
                if (menu_number == 1){
                    flag = 0;
                    display_info(flag);
                    while (true){
                        butt1.tick();
                        buttNext.tick();
                        buttPrevious.tick();
                        if (buttNext.isSingle()){
                            flag = flag + 1;
                            if (flag > 13){
                              flag = 0;
                            }
                            display_info(flag);
                        }
                        if (buttPrevious.isSingle()){
                            flag = flag - 1;
                            if (flag < 0){
                              flag = 13;
                            }
                            if (flag > 13){
                              flag = 0;
                            }
                            display_info(flag);
                        }
                        if (butt1.isSingle()){
                            display.clearDisplay();
                            display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
                            display.display();
                            wifi_channel(flag);
                        }
                    }
                }
                display.clearDisplay();
                display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
                display.display();
                wifi_jam();
            }
        }
    }
}
void access_poin_off(){
    settingsHandler(html_pls_reboot);
    storeEEPROMAndSet(8, 1, access_point);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 64, WHITE);
    display.display();
    delay(1000);
    ESP.restart();
}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    // Initialize EEPROM values if unset
    for (int i = 0; i < 10; ++i) {
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
    logo = EEPROM.read(7);
    access_point = EEPROM.read(8);
    buttons = EEPROM.read(9);
    if (access_point == 0){
        String current_ssid = getSSIDFromEEPROM();
        String current_password = getPasswordFromEEPROM();
        
        WiFi.softAP(current_ssid.c_str(), current_password.c_str());

        // Register routes
        registerRoute("/", handleRoot);
        registerRoute("/bluetooth_jam", []() { jamHandler(html_bluetooth_jam, bluetooth_jam, bitmap_bluetooth_jam); });
        registerRoute("/drone_jam", []() { jamHandler(html_drone_jam, drone_jam, bitmap_drone_jam); });
        registerRoute("/wifi_jam", []() { jamHandler(html_wifi_jam, wifi_jam, bitmap_wifi_jam); });
        registerRoute("/ble_jam", []() { jamHandler(html_ble_jam, ble_jam, bitmap_ble_jam); });
        registerRoute("/zigbee_jam", []() { jamHandler(html_zigbee_jam, zigbee_jam, bitmap_zigbee_jam); });
        registerRoute("/misc_jammer", []() { sendHtmlAndExecute(html_misc_jammer); });
        registerRoute("/misc_jam", miscChannelsHandler);
        registerRoute("/wifi_selected_jam", wifiChannelsHandler);

        registerRoute("/setting_bluetooth_jam", []() { settingsHandler(html_bluetooth_setings); });
        registerRoute("/setting_drone_jam", []() { settingsHandler(html_drone_setings); });
        registerRoute("/setting_separate_together", []() { settingsHandler(html_separate_or_together); });
        registerRoute("/setting_misc_jam", []() { settingsHandler(html_misc_setings); });
        registerRoute("/setting_logo", []() { settingsHandler(html_logo_setings); });
        registerRoute("/setting_buttons", []() { settingsHandler(html_buttons_settings); });
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
        registerRoute("/logo_on", []() { storeEEPROMAndSet(7, 0, logo); });
        registerRoute("/logo_off", []() { storeEEPROMAndSet(7, 1, logo); });
        registerRoute("/button_method_0", []() { storeEEPROMAndSet(9, 0, buttons); });
        registerRoute("/button_method_1", []() { storeEEPROMAndSet(9, 1, buttons); });
        registerRoute("/button_method_2", []() { storeEEPROMAndSet(9, 2, buttons); });
        registerRoute("/access_point_off", []() { access_poin_off(); });

        server.begin();
    }
    butt1.setTimeout(200);
    buttNext.setTimeout(200);
    buttPrevious.setTimeout(200);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    if (logo == 0){
        display.clearDisplay();
        display.drawBitmap(0, 0, bitmap_logo, 128, 64, WHITE);
        display.display();
        delay(2000);
    }
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_bluetooth_jammer, 128, 64, WHITE);
    display.display();
}

void loop() {
    butt1.tick();
    buttNext.tick();
    buttPrevious.tick();
    
    if (access_point == 0) server.handleClient();
    if (buttons == 0){
      if (butt1.isSingle()) {
          menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
          updateDisplay(menu_number);
      }
      if (butt1.isHolded()) {
          executeAction(menu_number);
      }
    }
    else if (buttons == 1){
      if (butt1.isSingle()) {
          executeAction(menu_number);
      }
      if (buttNext.isSingle()) {
          menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
          updateDisplay(menu_number);    
      }
    }
    else if (buttons == 2){
      if (butt1.isSingle()) {
          executeAction(menu_number);
      }

      if (buttNext.isSingle()) {
          menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
          updateDisplay(menu_number);    
      }

      if (buttPrevious.isSingle()) {
          menu_number = (menu_number - 1 + (access_point == 0 ? 6 : 7)) % (access_point == 0 ? 6 : 7);
          updateDisplay(menu_number);
      }
    }
}

void updateDisplay(int menuNum) {
    display.clearDisplay();
    const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jammer :
                            (menu_number == 1) ? bitmap_drone_jammer :
                            (menu_number == 2) ? bitmap_wifi_jammer : 
                            (menu_number == 3) ? bitmap_ble_jammer : 
                            (menu_number == 4) ? bitmap_zigbee_jammer : 
                            (menu_number == 5) ? bitmap_misc_jammer : bitmap_access_point;
    display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
    display.display();
}

void executeAction(int menuNum) {
    if (menuNum == 5) misc();
    display.clearDisplay();
    const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jam :
                            (menu_number == 1) ? bitmap_drone_jam :
                            (menu_number == 2) ? bitmap_wifi_all : 
                            (menu_number == 3) ? bitmap_ble_jam :
                            (menu_number == 4) ? bitmap_zigbee_jam :
                            (menu_number == 6) ? bitmap_pls_reboot : NULL;
    display.drawBitmap(0, 0, bitmap, 128, 64, WHITE);
    display.display();
    
    switch (menu_number) {
        case 0: bluetooth_jam(); break;
        case 1: drone_jam(); break;
        case 2: wifi_select(); break;
        case 3: ble_jam(); break;
        case 4: zigbee_jam(); break;
        case 6: storeEEPROMAndSet(8, 0, logo); break;
    }
}