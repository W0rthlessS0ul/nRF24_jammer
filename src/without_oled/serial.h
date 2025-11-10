#ifndef SERIAL_H
#define SERIAL_H

#include "options.h"
#include "html.h"
#include "jam.h"

extern String logotype;

bool SerialCommands();
void jamHandler(const char* html, void (*jamFunc)());
void storeEEPROMAndSet(int index, int value, int &targetVar);
void access_poin_off();
void updateDisplay(int menuNum);

#endif