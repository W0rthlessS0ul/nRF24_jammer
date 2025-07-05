// Пример использования библиотеки GyverButton
// опрос 5 кнопок в автоматическом режиме

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

  // устанавливаем опрос на автоматический
  butt1.setTickMode(AUTO);
  butt2.setTickMode(AUTO);
  butt3.setTickMode(AUTO);
  butt4.setTickMode(AUTO);
  butt5.setTickMode(AUTO);
}

void loop() {
  // проверяем одиночный клик
  // tick уже сидит внутри опроса
  if (butt1.isClick()) Serial.println("Button 1");
  if (butt2.isClick()) Serial.println("Button 2");
  if (butt3.isClick()) Serial.println("Button 3");
  if (butt4.isClick()) Serial.println("Button 4");
  if (butt5.isClick()) Serial.println("Button 5");
}
