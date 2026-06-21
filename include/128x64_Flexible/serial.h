#ifndef SERIAL_H
#define SERIAL_H

#include "attack.h"
#include "bitmap.h"
#include "config.h"
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
bool        handleSetArray(String *args, int argCount, bool web_command, String &response);

void attackHandler(String htmlResponse, void (*attackFunc)(), const uint8_t *bitmap);
void attackScanHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap);
void storeEEPROMAndSet(const char *index, int value, int &targetVar);
void access_poin_off();
void updateDisplay(int menuNum);
void storeEEPROMAndReset(const char *index, int value, int &targetVar);
void sendHtmlAndExecute(const char *htmlResponse, void (*action)());

#endif