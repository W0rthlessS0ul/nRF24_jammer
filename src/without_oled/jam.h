#ifndef JAM_H
#define JAM_H

#include "options.h"
#include "serial.h"

void VSPI_init();
void HSPI_init();
void bluetooth_jam();
void drone_jam();
void ble_jam();
void wifi_jam();
void wifi_channel(int channel);
void zigbee_jam();
void misc_jam(int channel1, int channel2);

#endif