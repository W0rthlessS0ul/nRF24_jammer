/*
   Пример использования библиотеки GyverButton, все возможности в одном скетче
   Данный скетч показывает, как быть в сложном "не сквозном" коде с кучей delay и замкнутых циклов
   Здесь кнопка опрашивается каждые 10 миллисекунд независимо от того, что происходит у вас в коде
   Используется библиотека TimerOne https://github.com/PaulStoffregen/TimerOne
*/

#define BTN_PIN 3				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)

#include "GyverButton.h"
#include "TimerOne.h"
GButton butt1(BTN_PIN);
int value = 0;

void setup() {
  Serial.begin(9600);

  Timer1.initialize(10000);           // установка таймера на каждые 10000 микросекунд (== 10 мс)
  Timer1.attachInterrupt(timerIsr);   // запуск таймера
}

void timerIsr() {   // прерывание таймера
  butt1.tick();     // отработка теперь находится здесь
}

void loop() {
  if (butt1.isClick()) Serial.println("Click");         // проверка на один клик
  if (butt1.isSingle()) Serial.println("Single");       // проверка на один клик
  if (butt1.isDouble()) Serial.println("Double");       // проверка на двойной клик
  if (butt1.isTriple()) Serial.println("Triple");       // проверка на тройной клик

  if (butt1.hasClicks())                                // проверка на наличие нажатий
    Serial.println(butt1.getClicks());                  // получить (и вывести) число нажатий

  if (butt1.isPress()) Serial.println("Press");         // нажатие на кнопку (+ дебаунс)
  if (butt1.isRelease()) Serial.println("Release");     // отпускание кнопки (+ дебаунс)
  if (butt1.isHolded()) Serial.println("Holded");       // проверка на удержание
  //if (butt1.isHold()) Serial.println("Hold");         // возвращает состояние кнопки

  if (butt1.isStep()) {                                 // если кнопка была удержана (это для инкремента)
    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
    Serial.println(value);                              // для примера выведем в порт
  }
}
