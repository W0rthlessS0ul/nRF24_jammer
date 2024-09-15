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
int ble_channels[] = {2, 26, 80};
int menu_number = 0;
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
    radio.setChannel(random(40));
    radio1.setChannel(random(40, 80));
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
void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/bluetooth_jam", bluetooth_jammer);
  server.on("/drone_jam", drone_jammer);
  server.on("/wifi_jam", wifi_jammer);
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
    menu_number = (menu_number + 1) % 3;
    display.clearDisplay();
    const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jammer :
                            (menu_number == 1) ? bitmap_drone_jammer : bitmap_wifi_jammer;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
    display.display();
  }

  if (butt1.isHolded()) {
    display.clearDisplay();
    const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jam :
                            (menu_number == 1) ? bitmap_drone_jam : bitmap_wifi_jam;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
    display.display();
    HSPI_init();
    VSPI_init();
    
    while (true) {
      if (menu_number == 0) {
        radio.setChannel(random(40));
        radio1.setChannel(random(40, 80));
      } else if (menu_number == 1) {
        radio.setChannel(random(62));
        radio1.setChannel(random(62, 125));
      } else if (menu_number == 2) {
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
  }
}