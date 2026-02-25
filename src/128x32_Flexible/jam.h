#ifndef JAM_H
#define JAM_H

#include "options.h"
#include "serial.h"
#include "scan.h"

void HSPI_init();
void bluetooth_jam();
void drone_jam();
void ble_advertising_jam();
void ble_data_jam();
void wifi_jam();
void wifi_scan_jam();
void wifi_channel(int channel);
void zigbee_jam();
void misc_jam(int channel1, int channel2);

#endif