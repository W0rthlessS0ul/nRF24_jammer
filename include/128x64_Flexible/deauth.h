#ifndef DEAUTH_H
#define DEAUTH_H

#include "config.h"
#include "serial.h"

String getSSIDFromEEPROM();
String getPasswordFromEEPROM();

void wifi_deauth_all();
void wifi_deauth_scan();
void wifi_deauth_channel(int channel);

#endif