// Пример использования библиотеки GyverButton, все возможности в одном скетче.

#define BTN_PIN 3				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)

#include "GyverButton.h"
GButton butt1(BTN_PIN);

// Варианты инициализации:
// GButton btn;               // без привязки к пину (виртуальная кнопка) и без указания типа (по умолч. HIGH_PULL и NORM_OPEN)
// GButton btn(пин);          // с привязкой к пину и без указания типа (по умолч. HIGH_PULL и NORM_OPEN)
// GButton btn(пин, тип подключ.);    // с привязкой к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и без указания типа кнопки (по умолч. NORM_OPEN)
// GButton btn(пин, тип подключ., тип кнопки);      // с привязкой к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и типа кнопки (NORM_OPEN / NORM_CLOSE)
// GButton btn(BTN_NO_BTN_PIN, тип подключ., тип кнопки); // без привязки к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и типа кнопки (NORM_OPEN / NORM_CLOSE)

int value = 0;

void setup() {
  Serial.begin(9600);

  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)

  // HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC (BTN_PIN --- КНОПКА --- GND)
  // LOW_PULL  - кнопка подключена к VCC, пин подтянут к GND
  // по умолчанию стоит HIGH_PULL
  butt1.setType(HIGH_PULL);

  // NORM_OPEN - нормально-разомкнутая кнопка
  // NORM_CLOSE - нормально-замкнутая кнопка
  // по умолчанию стоит NORM_OPEN
  butt1.setDirection(NORM_OPEN);
}

void loop() {
  butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться

  if (butt1.isClick()) Serial.println("Click");         // проверка на один клик
  if (butt1.isSingle()) Serial.println("Single");       // проверка на один клик
  if (butt1.isDouble()) Serial.println("Double");       // проверка на двойной клик
  if (butt1.isTriple()) Serial.println("Triple");       // проверка на тройной клик

  if (butt1.hasClicks())                                // проверка на наличие нажатий
    Serial.println(butt1.getClicks());                  // получить (и вывести) число нажатий

  if (butt1.isPress()) Serial.println("Press");         // нажатие на кнопку (+ дебаунс)
  if (butt1.isRelease()) Serial.println("Release");     // отпускание кнопки (+ дебаунс)
  if (butt1.isHold()) {									// если кнопка удерживается
    Serial.print("Holding ");							// выводим пока удерживается
    Serial.println(butt1.getHoldClicks());				// можно вывести количество кликов перед удержанием!
  }
  if (butt1.isHold()) Serial.println("Holding");        // проверка на удержание
  //if (butt1.state()) Serial.println("Hold");          // возвращает состояние кнопки

  if (butt1.isStep()) {                                 // если кнопка была удержана (это для инкремента)
    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
    Serial.println(value);                              // для примера выведем в порт
  }
}
