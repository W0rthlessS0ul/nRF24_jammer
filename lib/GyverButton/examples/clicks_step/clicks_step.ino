// ловим степ с накликиванием
#define BTN_PIN 0				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#include "GyverButton.h"
GButton butt1(BTN_PIN);

void setup() {
  Serial.begin(9600);
}

void loop() {
  butt1.tick();

  // удержание
  if (butt1.isStep()) {
    Serial.println("0x");
  }

  // один клик + удержание
  if (butt1.isStep(1)) {
    Serial.println("1x");
  }

  // два клика + удержание
  if (butt1.isStep(2)) {
    Serial.println("2x");
  }
}
