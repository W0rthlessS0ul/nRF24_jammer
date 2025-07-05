// Пример использования библиотеки GyverButton с аналоговой клавиатурой
// аналоговая клавиатура подключена на А7
// Схему смотри на странице библиотеки https://alexgyver.ru/gyverbutton/
// также она есть в папке с примером

#include "GyverButton.h"

// создаём кнопки без привязки к пину
GButton myButt1;
GButton myButt2;
GButton myButt3;

void setup() {
  Serial.begin(9600);
  // меняем тип на LOW_PULL, потому что по умолчанию стоит HIGH_PULL
  myButt1.setType(LOW_PULL);
  myButt2.setType(LOW_PULL);
  myButt3.setType(LOW_PULL);
}

void loop() {
  // читаем значение
  int analog = analogRead(7);

  // для начала нужно вывести и запомнить значение для каждой кнопки
  //Serial.println(analog);

  // проверяем у каждой кнопки свой диапазон (+- 20 от полученного значения)
  myButt1.tick(analog < 860 && analog > 820);
  myButt2.tick(analog < 740 && analog > 700);
  myButt3.tick(analog < 650 && analog > 600);

  // проверка на удержание, например
  if (myButt1.isHolded()) {
    Serial.println("hold 1");
  }
  if (myButt2.isHolded()) {
    Serial.println("hold 2");
  }
  if (myButt3.isHolded()) {
    Serial.println("hold 3");
  }
  
  delay(10);  // задержка тут не нужна, чисто для вывода
}
