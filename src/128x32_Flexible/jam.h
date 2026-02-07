#ifndef JAM_H
#define JAM_H

#include "options.h"
#include "serial.h"

String getSSIDFromEEPROM();
String getPasswordFromEEPROM();

void HSPI_init();
void bluetooth_jam();
void drone_jam();
void ble_jam();
void wifi_jam();
int scan_wifi_channels(int* channels, bool mode);
void wifi_scan_jam();
void wifi_channel(int channel);
void zigbee_jam();
void misc_jam(int channel1, int channel2);

#endif