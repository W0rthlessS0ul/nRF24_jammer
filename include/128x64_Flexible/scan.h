#ifndef SCAN_H
#define SCAN_H

#include "config.h"

String getSSIDFromEEPROM();
String getPasswordFromEEPROM();

int scan_wifi_APs(int *channels, bool mode);
int scan_BLE();

#endif