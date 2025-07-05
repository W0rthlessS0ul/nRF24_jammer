![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![author](https://img.shields.io/badge/author-AlexGyver-informational.svg)
# GyverButton
Библиотека для многофункциональной отработки нажатия кнопки
**ВНИМАНИЕ, БИБЛИОТЕКА УСТАРЕЛА! ИСПОЛЬЗУЙ БИБЛИОТЕКУ [EncButton](https://github.com/GyverLibs/EncButton)**
- Работа с нормально замкнутыми и нормально разомкнутыми кнопками
- Работа с подключением PULL_UP и PULL_DOWN
- Опрос кнопки с программным антидребезгом контактов (настраиваемое время)	
- Отработка нажатия, удерживания, отпускания, клика по кнопке (+ настройка таймаутов)	
- Отработка одиночного, двойного и тройного нажатия (вынесено отдельно)
- Отработка любого количества нажатий кнопки (функция возвращает количество нажатий)	
- Функция изменения значения переменной с заданным шагом и заданным интервалом по времени
- Возможность работы с "виртуальными" кнопками (все возможности библиотеки используются для матричных и резистивных клавиатур)

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

### Документация
К библиотеке есть [расширенная документация](https://alexgyver.ru/GyverButton/)

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverButton** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverButton/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Инициализация
```cpp
GButton btn;                     // без привязки к пину (виртуальная кнопка) и без указания типа (по умолч. HIGH_PULL и NORM_OPEN)
GButton btn(пин);                // с привязкой к пину и без указания типа (по умолч. HIGH_PULL и NORM_OPEN)
GButton btn(пин, тип подключ.);  // с привязкой к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и без указания типа кнопки (по умолч. NORM_OPEN)
GButton btn(пин, тип подключ., тип кнопки);         // с привязкой к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и типа кнопки (NORM_OPEN / NORM_CLOSE)
GButton btn(BTN_NO_PIN, тип подключ., тип кнопки);  // без привязки к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и типа кнопки (NORM_OPEN / NORM_CLOSE)
```

<a id="usage"></a>
## Использование
```cpp
void setDebounce(uint16_t debounce);        // установка времени антидребезга (по умолчанию 80 мс)
void setTimeout(uint16_t timeout);          // установка таймаута удержания (по умолчанию 300 мс)
void setClickTimeout(uint16_t timeout);      // установка таймаута между кликами (по умолчанию 500 мс)  
void setStepTimeout(uint16_t step_timeout); // установка таймаута между инкрементами (по умолчанию 400 мс)  
void setType(uint8_t type);     // установка типа кнопки (HIGH_PULL - подтянута к питанию, LOW_PULL - к gnd)  
void setDirection(uint8_t dir); // установка направления (разомкнута/замкнута по умолчанию - NORM_OPEN, NORM_CLOSE)  
  
void setTickMode(uint8_t tickMode); // (MANUAL / AUTO) ручной или автоматический опрос кнопки функцией tick()  
// MANUAL - нужно вызывать функцию tick() вручную                            
// AUTO - tick() входит во все остальные функции и опрашивается сама
  
void tick();               // опрос кнопки  
void tick(boolean state);  // опрос внешнего значения (0 нажато, 1 не нажато) (для матричных, резистивных клавиатур и джойстиков)
  
boolean isPress();    // возвращает true при нажатии на кнопку. Сбрасывается после вызова
boolean isRelease();  // возвращает true при отпускании кнопки. Сбрасывается после вызова
boolean isClick();    // возвращает true при клике. Сбрасывается после вызова
boolean isHolded();   // возвращает true при удержании дольше timeout. Сбрасывается после вызова
boolean isHold();     // возвращает true при нажатой кнопке, не сбрасывается
boolean state();      // возвращает состояние кнопки
boolean isSingle();   // возвращает true при одиночном клике. Сбрасывается после вызова
boolean isDouble();   // возвращает true при двойном клике. Сбрасывается после вызова
boolean isTriple();   // возвращает true при тройном клике. Сбрасывается после вызова
  
boolean hasClicks();  // проверка на наличие кликов. Сбрасывается после вызова
uint8_t getClicks();  // вернуть количество кликов
uint8_t getHoldClicks();// вернуть количество кликов, предшествующее удерживанию
  
boolean isStep();     // возвращает true по таймеру setStepTimeout, смотри пример
void resetStates();   // сбрасывает все is-флаги и счётчики
```

<a id="example"></a>
## Пример
Остальные примеры смотри в **examples**!
```cpp
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
```

<a id="versions"></a>
## Версии
- v2.15: Добавлена возможность объявить кнопку без привязки к пину	
- v3.0: Ускорен и оптимизирован код, переделана инициализация, дополнены примеры	
- v3.1: isStep может принимать количество кликов, сделанных перед ним (см. пример clicks_step)	
- v3.2: Добавлен метод getHoldClicks() - вернуть количество кликов, предшествующее удерживанию	
- v3.3: Мелкие исправления
- v3.4: Добавлен метод resetStates(), сбрасывающий состояния и счётчики
- v3.5: увелична производительность для AVR Arduino
- v3.6: добавлен отдельный класс для работы с аналоговыми клавиатурами, см пример analogKeyboardG
- v3.7: исправления от Dryundel: 
    - Любой таймаут удержания
    - Single, Double и Triple теперь не мешают hasClicks и getClicks и работают совместно
    - isStep() тоже теперь ничего не мешает и он работает более корректно
- v3.8: исправления от Dryundel

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!