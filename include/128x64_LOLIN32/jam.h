#pragma once
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
void wifi_channel(int channel);
void wifi_deauth_channel(int channel);
void zigbee_jam();
void misc_jam(int channel1, int channel2);
