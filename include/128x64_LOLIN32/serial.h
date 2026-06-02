#pragma once
#include "config.h"

bool SerialCommands();
void handleSerial();
void printSerialHelp();
void printSerialStatus();
void handleSerialSettings();
void handleSerialSetNRF(String input);
void handleSerialSetBTN(String input);
void handleSerialSetAP(String input);
void handleSerialSetDisplay(String input);
void handleSerialSetMenu(String input);
void handleSerialSetLogo(String input);
void handleSerialSetSepTog(String input);
void handleSerialSetBTJam(String input);
void handleSerialSetDrJam(String input);
void handleSerialSetWFJam(String input);
void handleSerialSetMiscJam(String input);
void handleSerialSetPALevel(String input);
void handleSerialSetCECSN(String input);
