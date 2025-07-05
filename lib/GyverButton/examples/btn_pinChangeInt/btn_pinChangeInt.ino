// пример с прерываниями pinChangeInterrupt (прерывания на любом пине)
// только для ATmega328 (UNO, Nano, Pro Mini)

#define BTN_PIN 3

#include "GyverButton.h"
GButton butt1(BTN_PIN);

void setup() {
  Serial.begin(9600);

  // настроить PCINT
  attachPCINT(BTN_PIN);
}

void loop() {
  butt1.tick();   // обязательно нужен отработчик ещё и в loop, чтобы работали таймауты
  
  if (butt1.isClick()) Serial.println("Click");         // проверка на один клик
  if (butt1.isSingle()) Serial.println("Single");       // проверка на один клик
  if (butt1.isDouble()) Serial.println("Double");       // проверка на двойной клик
  if (butt1.isTriple()) Serial.println("Triple");       // проверка на тройной клик
}

// функция для настройки PCINT для ATmega328 (UNO, Nano, Pro Mini)
uint8_t attachPCINT(uint8_t pin) {
  if (pin < 8) { // D0-D7 // PCINT2
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << pin);
    return 2;
  }
  else if (pin > 13) { //A0-A5  // PCINT1
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << pin - 14);
    return 1;
  }
  else { // D8-D13  // PCINT0
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << pin - 8);
    return 0;
  }
}

// Векторы PCINT, нужно кинуть сюда тики
// не обязательно в каждый вектор, достаточно в тот, который задействован
// пины 0-7: PCINT2
// пины 8-13: PCINT0
// пины A0-A5: PCINT1
ISR(PCINT0_vect) {
  //butt1.tick();
}
ISR(PCINT1_vect) {
  //butt1.tick();
}
ISR(PCINT2_vect) {
  butt1.tick();
}
