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

void jamHandler(const char* htmlResponse, const uint8_t* bitmap, void (*jamFunction)()) {
    server.send(200, "text/html", htmlResponse);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
    display.display();
    jamFunction();
}

void bluetooth_jammer() {
    jamHandler(html_bluetooth_jam, bitmap_bluetooth_jam, bluetooth_jam);
}
void drone_jammer() {
    jamHandler(html_drone_jam, bitmap_drone_jam, drone_jam);
}
void wifi_jammer() {
    jamHandler(html_wifi_jam, bitmap_wifi_jam, wifi_jam);
}
void ble_jammer() {
    jamHandler(html_ble_jam, bitmap_ble_jam, ble_jam);
}
void zigbee_jammer() {
    jamHandler(html_zigbee_jam, bitmap_zigbee_jam, zigbee_jam);
}
void misc_jammer() {
    server.send(200, "text/html", html_misc_jammer);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_misc_jammer, 128, 32, WHITE);
    display.display();
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
void setting_logo() { settingsHandler(html_logo_setings); }

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
void logo_on(){ storeEEPROM(7, 0); logo = 0;}
void logo_off(){ storeEEPROM(7, 1); logo = 1;}

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
        display.println(String(channel2-channel1));
        display.setCursor(100, 10);
        display.println(String(channel2));
        display.setCursor(0, 20);
        display.println(info);
        display.display();
    };

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display_info("");

    while (true) {
        butt1.tick();

        if (butt1.isSingle()) {
            if (flag == 0) {
                channel1++;
                if (channel1 > 125) {
                    channel1 = 0;
                }
            } else if (flag == 1) {
                channel2++;
                if (channel2 > 125) {
                    channel2 = 0;
                }
            }
            display_info(""); 
        } 
        else if (butt1.isHold()) {
            if (flag == 0) {
                channel1++;
                if (channel1 > 125) {
                    channel1 = 0;
                }
            } else if (flag == 1) {
                channel2++;
                if (channel2 > 125) {
                    channel2 = 0;
                }
            }
            display_info(""); 
            delay(100);
        }
        else if (butt1.isDouble()) {
            if (flag == 0) {
                flag = 1;
            } else {
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
    for (int i = 0; i < 8; ++i) {
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
    logo = EEPROM.read(7);
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
    server.on("/setting_logo", setting_logo);
    server.on("/logo_on", logo_on);
    server.on("/logo_off", logo_off);

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