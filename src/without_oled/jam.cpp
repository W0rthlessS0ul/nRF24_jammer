#include "jam.h"

void DeinitRadios(bool stopConstCarrier)
{
  if (stopConstCarrier){
    radio.stopConstCarrier();
    radio1.stopConstCarrier();
  }
  radio.powerDown();
  radio1.powerDown();
}
void VSPI_deinit()
{
  if (sp != nullptr) {
    sp->end();
    delete sp;
    sp = nullptr;
  }
}
void VSPI_init() {
  sp = new SPIClass(VSPI);
  sp->begin();
  sp->setFrequency(16000000);
  sp->setBitOrder(MSBFIRST);
  sp->setDataMode(SPI_MODE0);
  radio1.begin(sp);
  radio1.setAutoAck(false);
  radio1.stopListening();
  radio1.setRetries(0, 0);
  radio1.setPALevel(RF24_PA_MAX, true);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setCRCLength(RF24_CRC_DISABLED);
}
void HSPI_deinit()
{
  if (hp != nullptr) {
    hp->end();
    delete hp;
    hp = nullptr;
  }
}
void HSPI_init() {
  hp = new SPIClass(HSPI);
  hp->begin();
  hp->setFrequency(16000000);
  hp->setBitOrder(MSBFIRST);
  hp->setDataMode(SPI_MODE0);
  radio.begin(hp);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
}
void bluetooth_jam(){
  HSPI_init();
  VSPI_init();
  radio.startConstCarrier(RF24_PA_MAX, 45);
  radio1.startConstCarrier(RF24_PA_MAX, 45);
  bool SerialStop = true;
  while (SerialStop){
    SerialStop = SerialCommands();
    if (Separate_or_together == 0){
      if (bluetooth_jam_method == 0){
        for (int i = 0; i < 22; i++) {
          radio.setChannel(bluetooth_channels[i]);
          radio1.setChannel(bluetooth_channels_reversed[i]);
        }
      }
      if (bluetooth_jam_method == 1){
        radio.setChannel(random(40));
        radio1.setChannel(random(40, 80));
      }
      if (bluetooth_jam_method == 2){
        for (int i = 0; i < 80; i++) {
          radio.setChannel(i);
          radio1.setChannel(i-79);
        }
      }
    }
    if (Separate_or_together == 1){
      if (bluetooth_jam_method == 0){
        for (int i = 0; i < 22; i++) {
          radio.setChannel(bluetooth_channels[i]);
          radio1.setChannel(bluetooth_channels[i]);
        }
      }
      if (bluetooth_jam_method == 1){
        int random_channel = random(80);
        radio.setChannel(random_channel);
        radio1.setChannel(random_channel);
      }
      if (bluetooth_jam_method == 2){
        for (int i = 0; i < 80; i++) {
          radio.setChannel(i);
          radio1.setChannel(i);
        }
      }
    }
  }
  DeinitRadios(true);
  HSPI_deinit();
  VSPI_deinit();
}
void drone_jam(){
  HSPI_init();
  VSPI_init();
  radio.startConstCarrier(RF24_PA_MAX, 45);
  radio1.startConstCarrier(RF24_PA_MAX, 45);
  bool SerialStop = true;
  while (SerialStop){
    SerialStop = SerialCommands();
    if (Separate_or_together == 0){
      if (drone_jam_method == 0){
        radio.setChannel(random(64));
        radio1.setChannel(random(64, 125));
      }
      if (drone_jam_method == 1){
        for (int i = 0; i < 125; i++){
          radio.setChannel(i);
          radio1.setChannel(i-125);
        }
      }
    }
    if (Separate_or_together == 1){
      if (drone_jam_method == 0){
        int random_channel = random(125);
        radio.setChannel(random_channel);
        radio1.setChannel(random_channel);
      }
      if (drone_jam_method == 1){
        for (int i = 0; i < 125; i++){
          radio.setChannel(i);
          radio1.setChannel(i);
        }
      }
    }
  }
  DeinitRadios(true);
  HSPI_deinit();
  VSPI_deinit();
}
void ble_jam(){
  HSPI_init();
  VSPI_init();
  bool SerialStop = true;
  while (SerialStop){
    SerialStop = SerialCommands();
    if (Separate_or_together == 0){
      for (int i = 0; i < 3; i++){
        radio.setChannel(ble_channels[i]);
        radio1.setChannel(ble_channels_reversed[i]);
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
    }
    if (Separate_or_together == 1){
      for (int i = 0; i < 3; i++){
        radio.setChannel(ble_channels[i]);
        radio1.setChannel(ble_channels[i]);
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
  VSPI_deinit();
}
void wifi_jam(){
  HSPI_init();
  VSPI_init();
  bool SerialStop = true;
  while (SerialStop){
    if (Separate_or_together == 0){
      for (int channel = 0; channel < 13; channel++) {
        for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
          SerialStop = SerialCommands();
          if (!SerialStop)
            break;
          radio.setChannel(i);
          radio1.setChannel(83-i);
          radio.writeFast(&jam_text, sizeof(jam_text));
          radio1.writeFast(&jam_text, sizeof(jam_text));
        }
        if (!SerialStop)
          break;
      }
    }
    if (Separate_or_together == 1){
      for (int channel = 0; channel < 13; channel++) {
        for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
          SerialStop = SerialCommands();
          if (!SerialStop)
            break;
          radio.setChannel(i);
          radio1.setChannel(i);
          radio.writeFast(&jam_text, sizeof(jam_text));
          radio1.writeFast(&jam_text, sizeof(jam_text));
        }
        if (!SerialStop)
          break;
      }
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
  VSPI_deinit();
}
void wifi_channel(int channel){
  HSPI_init();
  VSPI_init();
  bool SerialStop = true;
  while (SerialStop){
    if (Separate_or_together == 0){
      for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
        SerialStop = SerialCommands();
        if (!SerialStop)
          break;
        radio.setChannel(i);
        radio1.setChannel((channel * 5 + 23) - (i - (channel * 5)));
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
      if (!SerialStop)
        break;
    }
    if (Separate_or_together == 1){
      for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
        SerialStop = SerialCommands();
        if (!SerialStop)
          break;
        radio.setChannel(i);
        radio1.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
      if (!SerialStop)
        break;
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
  VSPI_deinit();
}
void zigbee_jam(){
  HSPI_init();
  VSPI_init();
  bool SerialStop = true;
  while (SerialStop){
    if (Separate_or_together == 0){
      for (int channel = 11; channel < 27; channel++){
        for (int i = 5+5*(channel-11); i < (5+5*(channel-11))+6; i++){
          SerialStop = SerialCommands();
          if (!SerialStop)
            break;
          radio.setChannel(i);
          radio1.setChannel(85-i);
          radio.writeFast(&jam_text, sizeof(jam_text));
          radio1.writeFast(&jam_text, sizeof(jam_text));
        }
        if (!SerialStop)
          break;
      }
    }

    if (Separate_or_together == 1){
      for (int channel = 11; channel < 27; channel++){
        for (int i = 5+5*(channel-11); i < (5+5*(channel-11))+6; i++){
          SerialStop = SerialCommands();
          if (!SerialStop)
            break;
          radio.setChannel(i);
          radio1.setChannel(i);
          radio.writeFast(&jam_text, sizeof(jam_text));
          radio1.writeFast(&jam_text, sizeof(jam_text));
        }
        if (!SerialStop)
          break;
      }
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
  VSPI_deinit();
}
void misc_jam(int channel1, int channel2){
  HSPI_init();
  VSPI_init();
  if (misc_jam_method != 1) {
    radio.startConstCarrier(RF24_PA_MAX, 45);
    radio1.startConstCarrier(RF24_PA_MAX, 45);
  }
  bool SerialStop = true;
  while (SerialStop) {
    for (int i = 0; i <= channel2 - channel1; i++) {
      SerialStop = SerialCommands();
      if (!SerialStop)
        break;
      if (Separate_or_together == 0) {
        radio1.setChannel(channel2 - i);
      }
      else if (Separate_or_together == 1) {
        radio1.setChannel(channel1 + i);
      }
      radio.setChannel(channel1 + i);
      if (misc_jam_method == 1){
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
  if (misc_jam_method != 1)
    DeinitRadios(true);
  else
    DeinitRadios(false);
  HSPI_deinit();
  VSPI_deinit();
}