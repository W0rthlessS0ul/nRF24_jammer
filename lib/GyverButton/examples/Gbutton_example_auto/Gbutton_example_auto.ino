// Пример использования библиотеки GyverButton, все возможности в одном скетче.
// автоматический тик

#define BTN_PIN 3				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)

#include "GyverButton.h"
GButton butt1(BTN_PIN);
// GButton butt1(BTN_PIN, HIGH_PULL, NORM_OPEN); 	// можно инициализировать так

int value = 0;

void setup() {
  Serial.begin(9600);

  butt1.setDebounce(90);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)

  // HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC (BTN_PIN --- КНОПКА --- GND)
  // LOW_PULL  - кнопка подключена к VCC, пин подтянут к GND
  // по умолчанию стоит HIGH_PULL
  butt1.setType(HIGH_PULL);

  // NORM_OPEN - нормально-разомкнутая кнопка
  // NORM_CLOSE - нормально-замкнутая кнопка
  // по умолчанию стоит NORM_OPEN
  butt1.setDirection(NORM_OPEN);

  // MANUAL - нужно вызывать функцию tick() вручную
  // AUTO - tick() входит во все остальные функции и опрашивается сама!
  butt1.setTickMode(AUTO);
}

void loop() {
  // butt1.tick();  // НЕ НУЖНА, в этом режиме (AUTO) она входит в каждую функцию

  if (butt1.isClick()) Serial.println("Click");         // проверка на один клик
  if (butt1.isSingle()) Serial.println("Single");       // проверка на один клик
  if (butt1.isDouble()) Serial.println("Double");       // проверка на двойной клик
  if (butt1.isTriple()) Serial.println("Triple");       // проверка на тройной клик

  if (butt1.hasClicks())                                // проверка на наличие нажатий
    Serial.println(butt1.getClicks());                  // получить (и вывести) число нажатий

  if (butt1.isPress()) Serial.println("Press");         // нажатие на кнопку (+ дебаунс)
  if (butt1.isRelease()) Serial.println("Release");     // отпускание кнопки (+ дебаунс)
  if (butt1.isHolded()) Serial.println("Holded");       // проверка на удержание
  if (butt1.isHold()) Serial.println("Holding");        // проверка на удержание
  //if (butt1.state()) Serial.println("Hold");          // возвращает состояние кнопки

  if (butt1.isStep()) {                                 // если кнопка была удержана (это для инкремента)
    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
    Serial.println(value);                              // для примера выведем в порт
  }
}
