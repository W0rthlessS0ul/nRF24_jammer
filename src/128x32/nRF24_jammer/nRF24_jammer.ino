#include "SPI.h"
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

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display_info("");

    auto incrementChannel = [&](int& channel) {
        channel++;
        if (channel > 125) {
            channel = 0;
        }
    };

    while (true) {
        butt1.tick();

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

    registerRoute("/setting_bluetooth_jam", []() { settingsHandler(html_bluetooth_setings); });
    registerRoute("/setting_drone_jam", []() { settingsHandler(html_drone_setings); });
    registerRoute("/setting_separate_together", []() { settingsHandler(html_separate_or_together); });
    registerRoute("/setting_misc_jam", []() { settingsHandler(html_misc_setings); });
    registerRoute("/setting_logo", []() { settingsHandler(html_logo_setings); });

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

    server.begin();
    butt1.setTimeout(200);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if (logo == 0){
        display.clearDisplay();
        display.drawBitmap(0, 0, bitmap_logo, 128, 32, WHITE);
        display.display();
        delay(2000);
    }
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_bluetooth_jammer, 128, 32, WHITE);
    display.display();
}

void loop() {
    butt1.tick();
    server.handleClient();
    if (butt1.isSingle()) {
        menu_number = (menu_number + 1) % 6;
        display.clearDisplay();
        const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jammer :
                                (menu_number == 1) ? bitmap_drone_jammer :
                                (menu_number == 2) ? bitmap_wifi_jammer : 
                                (menu_number == 3) ? bitmap_ble_jammer : 
                                (menu_number == 4) ? bitmap_zigbee_jammer : bitmap_misc_jammer;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
    }
    
    if (butt1.isHolded()) {
        if (menu_number == 5){
          misc();
        }
        display.clearDisplay();
        const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jam :
                                (menu_number == 1) ? bitmap_drone_jam :
                                (menu_number == 2) ? bitmap_wifi_jam : 
                                (menu_number == 3) ? bitmap_ble_jam : bitmap_zigbee_jam;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
        
        switch (menu_number) {
            case 0: bluetooth_jam(); break;
            case 1: drone_jam(); break;
            case 2: wifi_jam(); break;
            case 3: ble_jam(); break;
            case 4: zigbee_jam(); break;
        }
    }
}
