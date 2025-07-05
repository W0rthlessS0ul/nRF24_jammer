/*
   Пример использования библиотеки GyverButton, отработка любого количества нажатий
*/

#define BTN_PIN 3				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#include "GyverButton.h"
GButton butt1(BTN_PIN);

void setup() {
  Serial.begin(9600);
  butt1.setTimeout(400);   // настройка таймаута на удержание и второй клик (по умолчанию 500 мс)
}

void loop() {
  butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться

  if (butt1.hasClicks())                  // проверка на наличие нажатий
    Serial.println(butt1.getClicks());    // получить (и вывести) число нажатий
}
