/*
    GyverButton - библиотека для многофункциональной отработки нажатия кнопки
    ВНИМАНИЕ, БИБЛИОТЕКА УСТАРЕЛА! ИСПОЛЬЗУЙ БИБЛИОТЕКУ EncButton https://github.com/GyverLibs/EncButton
    GitHub: https://github.com/GyverLibs/GyverButton
    Документация: https://alexgyver.ru/gyverbutton/
    Возможности:
    - Работа с нормально замкнутыми и нормально разомкнутыми кнопками
    - Работа с подключением PULL_UP и PULL_DOWN
    - Опрос кнопки с программным антидребезгом контактов (настраиваемое время)	
    - Отработка нажатия, удерживания, отпускания, клика по кнопке (+ настройка таймаутов)	
    - Отработка одиночного, двойного и тройного нажатия (вынесено отдельно)
    - Отработка любого количества нажатий кнопки (функция возвращает количество нажатий)	
    - Функция изменения значения переменной с заданным шагом и заданным интервалом по времени
    - Возможность работы с "виртуальными" кнопками (все возможности библиотеки используются для матричных и резистивных клавиатур)
    
    Версии:
    v2.15: Добавлена возможность объявить кнопку без привязки к пину	
    v3.0: Ускорен и оптимизирован код, переделана инициализация, дополнены примеры	
    v3.1: isStep может принимать количество кликов, сделанных перед ним (см. пример clicks_step)	
    v3.2: Добавлен метод getHoldClicks() - вернуть количество кликов, предшествующее удерживанию	
    v3.3: Мелкие исправления
    v3.4: Добавлен метод resetStates(), сбрасывающий состояния и счётчики
    v3.5: увелична производительность для AVR Arduino
    v3.6: добавлен отдельный класс для работы с аналоговыми клавиатурами, см пример analogKeyboardG
    v3.7: исправления от Dryundel: 
    - Любой таймаут удержания
    - Single, Double и Triple теперь не мешают hasClicks и getClicks и работают совместно
    - isStep() тоже теперь ничего не мешает и он работает более корректно
    v3.8: исправления от Dryundel:
    
*/
#ifndef GyverButton_h
#define GyverButton_h
#include <Arduino.h>

#if defined(__AVR__)
#define _buttonRead() bool(*_pin_reg & _bit_mask)
#else
#define _buttonRead() digitalRead(_PIN)
#endif

#pragma pack(push,1)
typedef struct {		
    bool btn_deb: 1;	
    bool hold_flag: 1;
    bool counter_flag: 1;
    bool isHolded_f: 1;
    bool isRelease_f: 1;
    bool isPress_f: 1;
    bool step_flag: 1;
    bool oneClick_f: 1;
    bool isOne_f: 1;
    bool inv_state: 1;
    bool mode: 1;
    bool type: 1;
    bool tickMode: 1;
    bool noPin: 1;
    bool counter_reset: 1;			//Добавлен
} GyverButtonFlags;
#pragma pack(pop)

#define BTN_NO_PIN -1
#define HIGH_PULL 0
#define LOW_PULL 1
#define NORM_OPEN 0
#define NORM_CLOSE 1
#define MANUAL 0
#define AUTO 1

// Варианты инициализации:
// GButton btn;							// без привязки к пину (виртуальная кнопка) и без указания типа (по умолч. LOW_PULL и NORM_OPEN)
// GButton btn(пин);					// с привязкой к пину и без указания типа (по умолч. HIGH_PULL и NORM_OPEN)
// GButton btn(пин, тип подключ.);		// с привязкой к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и без указания типа кнопки (по умолч. NORM_OPEN)
// GButton btn(пин, тип подключ., тип кнопки);			// с привязкой к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и типа кнопки (NORM_OPEN / NORM_CLOSE)
// GButton btn(BTN_NO_PIN, тип подключ., тип кнопки);	// без привязки к пину и указанием типа подключения (HIGH_PULL / LOW_PULL) и типа кнопки (NORM_OPEN / NORM_CLOSE)

class GButton {
public:
    GButton(int8_t pin = BTN_NO_PIN, bool type = HIGH_PULL, bool dir = NORM_OPEN);	// конструктор кнопки, принимает PIN пин, тип type (HIGH_PULL / LOW_PULL) и направление dir (NORM_OPEN / NORM_CLOSE)
    // HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC, pinMode - INPUT_PULLUP (по умолчанию)
    // LOW_PULL - кнопка подключена к VCC, пин подтянут к GND, pinMode - INPUT
    // NORM_OPEN - кнопка по умолчанию разомкнута (по умолчанию)
    // NORM_CLOSE - кнопка по умолчанию замкнута
    
    void setDebounce(uint16_t debounce);				// установка времени антидребезга (по умолчанию 80 мс)
    void setTimeout(uint16_t new_timeout);				// установка таймаута удержания (по умолчанию 300 мс)
    void setClickTimeout(uint16_t new_timeout);			// установка таймаута между кликами (по умолчанию 500 мс)	
    void setStepTimeout(uint16_t step_timeout);			// установка таймаута между инкрементами (по умолчанию 400 мс)	
    void setType(bool type);							// установка типа кнопки (HIGH_PULL - подтянута к питанию, LOW_PULL - к gnd)	
    void setDirection(bool dir);						// установка направления (разомкнута/замкнута по умолчанию - NORM_OPEN, NORM_CLOSE)	
    
    void setTickMode(bool tickMode);					// (MANUAL / AUTO) ручной или автоматический опрос кнопки функцией tick()	
    // MANUAL - нужно вызывать функцию tick() вручную														
    // AUTO - tick() входит во все остальные функции и опрашивается сама
    
    void tick();										// опрос кнопки	
    void tick(boolean state);							// опрос внешнего значения (1 нажато, 0 не нажато) (для матричных, резистивных клавиатур и джойстиков)
    
    boolean isPress();		// возвращает true при нажатии на кнопку. Сбрасывается после вызова
    boolean isRelease();	// возвращает true при отпускании кнопки. Сбрасывается после вызова
    boolean isClick();		// возвращает true при клике. Сбрасывается после вызова
    boolean isHolded();		// возвращает true при удержании дольше timeout. Сбрасывается после вызова
    boolean isHold();		// возвращает true при нажатой кнопке, не сбрасывается
    boolean state();		// возвращает состояние кнопки

    boolean isSingle();		// возвращает true при одиночном клике. Сбрасывается после вызова
    boolean isDouble();		// возвращает true при двойном клике. Сбрасывается после вызова
    boolean isTriple();		// возвращает true при тройном клике. Сбрасывается после вызова
    
    boolean hasClicks();	// проверка на наличие кликов. Сбрасывается после вызова
    uint8_t getClicks();	// вернуть количество кликов
    uint8_t getHoldClicks();// вернуть количество кликов, предшествующее удерживанию
    
    boolean isStep(byte clicks = 0); // возвращает true по таймеру setStepTimeout, смотри пример
    
    void resetStates();		// сбрасывает все is-флаги и счётчики
    
private:
    GyverButtonFlags flags;
    uint8_t _PIN = 0;
    uint16_t _debounce = 60;
    uint16_t _timeout = 500;
    uint16_t _click_timeout = 500;
    uint16_t _step_timeout = 400;
    uint8_t btn_counter = 0, last_counter = 0, last_hold_counter = 0;
    uint32_t btn_timer = 0;	
    bool btn_state = false;
    bool btn_flag = false;
#if defined(__AVR__)
    volatile uint8_t *_pin_reg;
    volatile uint8_t _bit_mask;
#endif
};
#endif