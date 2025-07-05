// Пример использования библиотеки GyverButton, все возможности в одном скетче.
// Дополнительный опрос по аппаратному прерыванию

#define BTN_PIN 3				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)

#include "GyverButton.h"
GButton butt1(BTN_PIN);
int value = 0;

void setup() {
  Serial.begin(9600);
  attachInterrupt(1, isr, CHANGE);

  butt1.setDebounce(80);      // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);      // настройка таймаута на удержание (по умолчанию 500 мс)
}

void isr() {
  butt1.tick();  // опрашиваем в прерывании, чтобы поймать нажатие в любом случае
}

void loop() {
  butt1.tick();  // опрашиваем в скетче, иначе не будут работать проверки по времени!

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
