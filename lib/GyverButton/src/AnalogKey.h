#ifndef AnalogKey_h
#define AnalogKey_h
/*
    Мини класс для работы с аналоговой клавиатурой
    GitHub: https://github.com/GyverLibs/AnalogKey
    - Встроенный простенький дебаунс
    - Оптимальный опрос пина по таймеру
    - Проверка конкретной кнопки или вывод номера нажатой
    
    AlexGyver, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License
*/

#define _AKEY_PERIOD 50   // период опроса в мс

template <byte PIN, byte AMOUNT>
class AnalogKey {
public:
    // подключает кнопку на указанное значение
    void attach(byte num, int value) {
        if (num >= AMOUNT) return;
        signals[num] = value;
    }

    // устанавливает окно сигнала (умолч. 20)
    void setWindow(int window) {
        _window = window;
    }
    
    // возвращает статус указанной кнопки
    bool status(byte num) {
        if (millis() - tmr > _AKEY_PERIOD) {
            tmr = millis();
            int thisRead = analogRead(PIN);
            if (abs(thisRead - _lastRead) < _window) _ready = true;
            else _ready = false;
            _lastRead = thisRead;		
        }
        if (_ready &&
                _lastRead > signals[num] - _window &&
                _lastRead < signals[num] + _window) return true;
        else return false;
    }
    
    // выводит номер нажатой кнопки или -1, если нажатых кнопок нет
    int pressed() {
        status(0);	// вызываем опрос
        for (byte i = 0; i < AMOUNT; i++) {
            if (_ready &&
                    _lastRead > signals[i] - _window &&
                    _lastRead < signals[i] + _window) return i;
        }
        return -1;
    }

private:
    int signals[AMOUNT];
    int _lastRead = 0;
    int _window = 20;
    bool _ready = false;
    uint32_t tmr;
};
#endif