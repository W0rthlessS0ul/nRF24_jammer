#ifndef SERIAL_H
#define SERIAL_H

#include "bitmap.h"
#include "config.h"
#include "deauth.h"
#include "html.h"
#include "jam.h"
#include "scan.h"

extern String logotype;

extern bool CommandsHandler(String command, bool web_command);
extern bool SerialCommands();
bool        handleJamCommand(String *args, int argCount, bool web_command, String &response);
bool        handleAttackCommand(String *args, int argCount, bool web_command, String &response);
bool        handleScanCommand(String *args, int argCount, bool web_command, String &response);
bool        handleSetCommand(String *args, int argCount, bool web_command, String &response);
bool        handleSetNrf24Command(String *args, int argCount, bool web_command, String &response);

void jamHandler(String htmlResponse, void (*jamFunc)(), const uint8_t *bitmap);
void attackHandler(String htmlResponse, void (*attackFunc)(), const uint8_t *bitmap);
void storeEEPROMAndSet(int index, int value, int &targetVar);
void access_poin_off();
void updateDisplay(int menuNum);
void storeEEPROMAndReset(int index, int value, int &targetVar);
void sendHtmlAndExecute(const char *htmlResponse, void (*action)());

#endif