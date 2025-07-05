#include "GyverButton.h"

// ==================== CONSTRUCTOR ====================
GButton::GButton(int8_t pin, bool type, bool dir) {
    if (pin != BTN_NO_PIN) {
        _PIN = (uint8_t)pin;
        flags.noPin = false;
    } else {
        flags.noPin = true;
    }
    setType(type);
    flags.mode = false;
    flags.tickMode = false;
    flags.inv_state = dir;
}

// ==================== SET ====================
void GButton::setDebounce(uint16_t debounce) {
    _debounce = debounce;
}
void GButton::setTimeout(uint16_t new_timeout) {
    _timeout = new_timeout;
}
void GButton::setClickTimeout(uint16_t new_timeout) {
    _click_timeout = new_timeout;
}
void GButton::setStepTimeout(uint16_t step_timeout) {
    _step_timeout = step_timeout;
}
void GButton::setType(bool type) {
    flags.type = type;
    if (!flags.noPin) {
        if (type) pinMode(_PIN, INPUT);
        else pinMode(_PIN, INPUT_PULLUP);
#if defined(__AVR__)
        _pin_reg = portInputRegister(digitalPinToPort(_PIN));
        _bit_mask = digitalPinToBitMask(_PIN);
#endif
    }	
}
void GButton::setDirection(bool dir) {
    flags.inv_state = dir;
}
void GButton::setTickMode(bool tickMode) {
    flags.tickMode = tickMode;
}

// ==================== IS ====================
boolean GButton::isPress() {
    if (flags.tickMode) GButton::tick();
    if (flags.isPress_f) {
        flags.isPress_f = false;
        return true;
    } else return false;
}
boolean GButton::isRelease() {
    if (flags.tickMode) GButton::tick();
    if (flags.isRelease_f) {
        flags.isRelease_f = false;
        return true;
    } else return false;
}
boolean GButton::isClick() {	
    if (flags.tickMode) GButton::tick();
    if (flags.isOne_f) {
        flags.isOne_f = false;
        return true;
    } else return false;
}
boolean GButton::isHolded() {
    if (flags.tickMode) GButton::tick();
    if (flags.isHolded_f) {
        flags.isHolded_f = false;
        return true;
    } else return false;
}
boolean GButton::isHold() {
    if (flags.tickMode) GButton::tick();
    if (flags.step_flag) return true;
    else return false;
}
boolean GButton::state() {
    if (flags.tickMode) GButton::tick();
    return btn_state;
}
boolean GButton::isSingle() {
    if (flags.tickMode) GButton::tick();
    if (flags.counter_flag && last_counter == 1) {
        //		last_counter = 0;
        //		flags.counter_flag = false;
        flags.counter_reset = true;	
        return true;
    } else return false;
}
boolean GButton::isDouble() {
    if (flags.tickMode) GButton::tick();
    if (flags.counter_flag && last_counter == 2) {
        //		flags.counter_flag = false;
        //		last_counter = 0;
        flags.counter_reset = true;
        return true;
    } else return false;
}
boolean GButton::isTriple() {
    if (flags.tickMode) GButton::tick();
    if (flags.counter_flag && last_counter == 3) {
        //		flags.counter_flag = false;
        //		last_counter = 0;
        flags.counter_reset = true;
        return true;
    } else return false;
}
boolean GButton::hasClicks() {
    if (flags.tickMode) GButton::tick();
    if (flags.counter_flag) {
        //		flags.counter_flag = false;
        flags.counter_reset = true;	
        return true;
    } else return false;
}
uint8_t GButton::getClicks() {
    //	byte thisCount = last_counter;			// Исключено	14.01.2021
    //		last_counter = 0;
    flags.counter_reset = true;	
    return last_counter;					//	return thisCount;	(замена)	14.01.2021
}
uint8_t GButton::getHoldClicks() {
    if (flags.tickMode) GButton::tick();
    return last_hold_counter;				//return flags.hold_flag ? last_hold_counter : 0;	(Замена)	 14.01.2021
}
boolean GButton::isStep(byte clicks) {
    if (flags.tickMode) GButton::tick();
    if (btn_counter == clicks && flags.step_flag && (millis() - btn_timer >= _step_timeout)) {
        btn_timer = millis();
        return true;
    }
    else return false;
}

void GButton::resetStates() {
    flags.isPress_f = false;
    flags.isRelease_f = false;
    flags.isOne_f = false;
    flags.isHolded_f = false;
    flags.step_flag = false;
    flags.counter_flag = false;
    last_hold_counter = 0;
    last_counter = 0;
}

// ==================== TICK ====================
void GButton::tick(boolean state) {
    flags.mode = true;
    btn_state = state ^ flags.inv_state;
    GButton::tick();
    flags.mode = false;
}

void GButton::tick() {	
    // читаем пин
    if (!flags.mode && !flags.noPin) btn_state = !_buttonRead() ^ (flags.inv_state ^ flags.type);
    
    uint32_t thisMls = millis();
    
    // нажатие
    if (btn_state && !btn_flag) {
        if (!flags.btn_deb) {
            flags.btn_deb = true;
            btn_timer = thisMls;
        } else {
            if (thisMls - btn_timer >= _debounce) {
                btn_flag = true;			
                flags.isPress_f = true;
                flags.oneClick_f = true;
            }
        }    
    } else {
        flags.btn_deb = false;
    }

    // отпускание
    if (!btn_state && btn_flag) {
        btn_flag = false;
        if (!flags.hold_flag) btn_counter++;
        flags.hold_flag = false;
        flags.isRelease_f = true;
        btn_timer = thisMls;
        if (flags.step_flag) {
            last_counter = 0;
            btn_counter = 0;
            flags.step_flag = false;	
        }		
        if (flags.oneClick_f) {
            flags.oneClick_f = false;
            flags.isOne_f = true;
        }
    }

    // кнопка удерживается
    if (btn_flag && btn_state && (thisMls - btn_timer >= _timeout) && !flags.hold_flag) {
        flags.hold_flag = true;
        last_hold_counter = btn_counter;
        //btn_counter = 0;
        //last_counter = 0;
        flags.isHolded_f = true;
        flags.step_flag = true;
        flags.oneClick_f = false;
        btn_timer = thisMls;
    }

    // обработка накликивания
    if ((thisMls - btn_timer >= _click_timeout) && (btn_counter != 0) &&  !btn_state) {    //И здесь еще добавлен !btn_state
        last_counter = btn_counter;
        btn_counter = 0;
        flags.counter_flag = true;
    }
    
    // сброс накликивания						//Добавлено
    if (flags.counter_reset) {
        last_counter = 0;
        flags.counter_flag = false;
        flags.counter_reset = false;
    }
}