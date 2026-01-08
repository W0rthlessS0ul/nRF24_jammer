#ifndef SERIAL_H
#define SERIAL_H

#include "options.h"
#include "html.h"
#include "bitmap.h"
#include "jam.h"

extern String logotype;

extern bool CommandsHandler(String command, bool web_command); 
extern bool SerialCommands();

void jamHandler(String htmlResponse, void (*jamFunc)(), const uint8_t* bitmap);
void storeEEPROMAndSet(int index, int value, int &targetVar);
void access_poin_off();
void updateDisplay(int menuNum);
void storeEEPROMAndReset(int index, int value, int &targetVar);
void sendHtmlAndExecute(const char *htmlResponse, void (*action)());

#endif