#include "jam.h"

void DeinitRadio(bool stopConstCarrier)
{
  if (stopConstCarrier){
    radio.stopConstCarrier();
  }
  radio.powerDown();
}

void HSPI_init()
{
  hp = new SPIClass(HSPI);
  hp->begin();
  hp->setFrequency(16000000);
  hp->setBitOrder(MSBFIRST);
  hp->setDataMode(SPI_MODE0);
  radio.begin(hp);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPayloadSize(5);
  radio.setAddressWidth(3);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
}
void HSPI_deinit()
{
  if (hp != nullptr) {
    hp->end();
    delete hp;
    hp = nullptr;
  }
}
void bluetooth_jam()
{
  HSPI_init();
  radio.startConstCarrier(RF24_PA_MAX, 45);
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    if (bluetooth_jam_method == 0)
    {
      for (int i = 0; i < 21; i++)
      {
        radio.setChannel(bluetooth_channels[i]);
      }
    }
    if (bluetooth_jam_method == 1)
    {
      int random_channel = random(80);
      radio.setChannel(random_channel);
    }
    if (bluetooth_jam_method == 2)
    {
      for (int i = 0; i < 80; i++)
      {
        radio.setChannel(i);
      }
    }
  }
  DeinitRadio(true);
  HSPI_deinit();
}
void drone_jam()
{
  HSPI_init();
  radio.startConstCarrier(RF24_PA_MAX, 45);
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    if (drone_jam_method == 0)
    {
      int random_channel = random(125);
      radio.setChannel(random_channel);
    }
    if (drone_jam_method == 1)
    {
      for (int i = 0; i < 125; i++)
      {
        radio.setChannel(i);
      }
    }
  }
  DeinitRadio(true);
  HSPI_deinit();
}
void ble_jam()
{
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    for (int i = 0; i < 3; i++)
    {
      radio.setChannel(ble_channels[i]);
      radio.writeFast(&jam_text, sizeof(jam_text));
    }
  }
  DeinitRadio(false);
  HSPI_deinit();
}
void wifi_jam()
{
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    for (int channel = 0; channel < 13; channel++)
    {
      for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++)
      {
        butt1.tick();
        SerialStop = SerialCommands();
        if (butt1.isSingle() || !SerialStop)
          break;
        radio.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
      }
      if (butt1.isSingle() || !SerialStop)
        break;
    }
  }
  DeinitRadio(false);
  HSPI_deinit();
}
void wifi_channel(int channel)
{
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++)
    {
      butt1.tick();
      SerialStop = SerialCommands();
      if (butt1.isSingle() || !SerialStop)
        break;
      radio.setChannel(i);
      radio.writeFast(&jam_text, sizeof(jam_text));
    }
  }
  DeinitRadio(false);
  HSPI_deinit();
}
void zigbee_jam()
{
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    for (int channel = 11; channel < 27; channel++)
    {
      for (int i = 5 + 5 * (channel - 11); i < (5 + 5 * (channel - 11)) + 6; i++)
      {
        butt1.tick();
        SerialStop = SerialCommands();
        if (butt1.isSingle() || !SerialStop)
          break;
        radio.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
      }
      if (butt1.isSingle() || !SerialStop)
        break;
    }
  }
  DeinitRadio(false);
  HSPI_deinit();
}
void misc_jam(int channel1, int channel2)
{
  HSPI_init();
  if (misc_jam_method != 1)
    radio.startConstCarrier(RF24_PA_MAX, 45);
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    for (int i = 0; i <= channel2 - channel1; i++)
    {
      radio.setChannel(channel1 + i);
      if (misc_jam_method == 1)
      {
        radio.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
  if (misc_jam_method != 1)
    DeinitRadio(true);
  else
  {
    DeinitRadio(false);
  }
  HSPI_deinit();
}