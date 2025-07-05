// новый пример работы с аналоговой клавиатурой
/*
  Сигналы кнопок
  1023
  927
  856
  783
  671
  632
  590
  560
  504
  480
  455
  440
  399
  319
  255
  230
*/
#include "AnalogKey.h"
// указываем пин и количество кнопок
AnalogKey<A0, 3> keys;

#include <GyverButton.h>
GButton btn1, btn2, btn3;

void setup() {
  Serial.begin(9600);
  // указываем сигналы кнопок аналоговой клавы
  keys.attach(0, 1023);
  keys.attach(1, 927);
  keys.attach(2, 856);
}

void loop() {
  // тикаем кнопки как статусы аналоговых
  btn1.tick(keys.status(0));
  btn2.tick(keys.status(1));
  btn3.tick(keys.status(2));
  
  if (btn1.isClick()) Serial.println("click 0");
  if (btn2.isClick()) Serial.println("click 1");
  if (btn3.isClick()) Serial.println("click 2");

  if (btn1.isHolded()) Serial.println("click 0");
  if (btn2.isHolded()) Serial.println("hold 1");
  if (btn3.isHolded()) Serial.println("hold 2");
}
