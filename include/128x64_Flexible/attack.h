#ifndef ATTACK_H
#define ATTACK_H

#include "config.h"
#include "serial.h"

void wifi_deauth_all();
void wifi_deauth_scan();
void wifi_deauth_channel(int channel);
void wifi_beacon_spam_random();
void wifi_beacon_spam_array();

#endif