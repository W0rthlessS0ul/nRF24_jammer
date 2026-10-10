#ifndef JAM_H
#define JAM_H

#include "attack.h"
#include "config.h"
#include "scan.h"
#include "serial.h"

void InitRadios();
void DeinitRadios(bool stopConstCarrier);
void HSPI_init();
void HSPI_deinit();

void bluetooth_jam();
void drone_jam();
void ble_advertising_jam();
void ble_data_jam();
void wifi_jam();
void wifi_scan_jam();
void wifi_channel();
void zigbee_jam();
void misc_jam();
bool jamHandler(void (*action)(), String htmlResponse = "", const unsigned char *bitmap = nullptr, bool CW = false, bool mode = false, bool isMisc = false, bool scan = false, int ch1 = 0, int ch2 = 0);

void updateDisplay(int menuNum);

#endif