#include "SPI.h"
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
#include "EEPROM.h"

void handleRoot() {
    server.send(200, "text/html", html);
}

void bluetooth_jammer() {
    server.send(200, "text/html", html_bluetooth_jam);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_bluetooth_jam, 128, 32, WHITE);
    display.display();
    bluetooth_jam();
}

void drone_jammer() {
    server.send(200, "text/html", html_drone_jam);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_drone_jam, 128, 32, WHITE);
    display.display();
    drone_jam();
}

void wifi_jammer() {
    server.send(200, "text/html", html_wifi_jam);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
    display.display();
    wifi_jam();
}

void ble_jammer() {
    server.send(200, "text/html", html_ble_jam);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_ble_jam, 128, 32, WHITE);
    display.display();
    ble_jam();
}

void setting_bluetooth_jam() {
    server.send(200, "text/html", html_bluetooth_setings);
}

void setting_drone_jam() {
    server.send(200, "text/html", html_drone_setings);
}

void setting_separate_together() {
    server.send(200, "text/html", html_separate_or_together);
}

void bluetooth_method_0() {
    EEPROM.write(0, 0);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void bluetooth_method_1() {
    EEPROM.write(0, 1);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void bluetooth_method_2() {
    EEPROM.write(0, 2);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void drone_method_0() {
    EEPROM.write(1, 0);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void drone_method_1() {
    EEPROM.write(1, 1);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void separate_or_together_method_0() {
    EEPROM.write(4, 0);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void separate_or_together_method_1() {
    EEPROM.write(4, 1);
    EEPROM.commit();
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);
    bluetooth_jam_method = EEPROM.read(0);
    drone_jam_method = EEPROM.read(1);
    ble_jam_method = EEPROM.read(2);
    wifi_jam_method = EEPROM.read(3);
    Separate_or_together = EEPROM.read(4);
    for (int i = 0; i < 5; i++) {
        if (EEPROM.read(i) == 255) {
            EEPROM.write(i, 0);
            EEPROM.commit();
        }
    }
    WiFi.softAP(ssid, password);
    server.on("/", handleRoot);
    server.on("/bluetooth_jam", bluetooth_jammer);
    server.on("/drone_jam", drone_jammer);
    server.on("/wifi_jam", wifi_jammer);
    server.on("/ble_jam", ble_jammer);
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
    server.begin();
    butt1.setTimeout(200);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_logo, 128, 32, WHITE);
    display.display();
    delay(2000);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_bluetooth_jammer, 128, 32, WHITE);
    display.display();
}
void loop() {
    butt1.tick();
    server.handleClient();
    if (butt1.isSingle()) {
        menu_number = (menu_number + 1) % 4;
        display.clearDisplay();
        const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jammer :
                                (menu_number == 1) ? bitmap_drone_jammer :
                                (menu_number == 2) ? bitmap_wifi_jammer : bitmap_ble_jammer;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
    }
    if (butt1.isHolded()) {
        display.clearDisplay();
        const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jam :
                                (menu_number == 1) ? bitmap_drone_jam :
                                (menu_number == 2) ? bitmap_wifi_jam : bitmap_ble_jam;

        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
        switch (menu_number) {
            case 0:
                bluetooth_jam();
                break;
            case 1:
                drone_jam();
                break;
            case 2:
                wifi_jam();
                break;
            case 3:
                ble_jam();
                break;
        }
    }
}
