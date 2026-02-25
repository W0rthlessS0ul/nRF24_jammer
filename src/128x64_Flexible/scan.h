#ifndef SCAN_H
#define SCAN_H

#include "options.h"

String getSSIDFromEEPROM();
String getPasswordFromEEPROM();

int scan_wifi_APs(int* channels, bool mode);

#endif