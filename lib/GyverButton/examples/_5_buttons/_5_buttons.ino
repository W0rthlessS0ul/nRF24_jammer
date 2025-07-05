// Пример использования библиотеки GyverButton
// опрос 5 кнопок в ручном режиме

// кнопки подключены к земле (PIN --- КНОПКА --- GND)
#define BTN1 3
#define BTN2 4
#define BTN3 5
#define BTN4 6
#define BTN5 7

#include "GyverButton.h"
GButton butt1(BTN1);
GButton butt2(BTN2);
GButton butt3(BTN3);
GButton butt4(BTN4);
GButton butt5(BTN5);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // тик в ручном режиме
  butt1.tick();
  butt2.tick();
  butt3.tick();
  butt4.tick();
  butt5.tick();

  // проверяем одиночный клик
  if (butt1.isClick()) Serial.println("Button 1");
  if (butt2.isClick()) Serial.println("Button 2");
  if (butt3.isClick()) Serial.println("Button 3");
  if (butt4.isClick()) Serial.println("Button 4");
  if (butt5.isClick()) Serial.println("Button 5");
}
