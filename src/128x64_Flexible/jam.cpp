#include "jam.h"

void InitRadios()
{
  for (int i = 0; i < nrf24_count; i++)
  {
    radios[i] = new RF24(ce_pins[i], csn_pins[i]);
  }
}
void DeinitRadios(bool stopConstCarrier)
{
  for (int i = 0; i < nrf24_count; i++){
    if (radios[i] != nullptr){
      if (stopConstCarrier){
        radios[i]->stopConstCarrier();
      }
      radios[i]->powerDown();
      delete radios[i];
      radios[i] = nullptr;
    }
  }
}
void HSPI_init()
{
  hp = new SPIClass(HSPI);
  hp->begin();
  hp->setFrequency(16000000);
  hp->setBitOrder(MSBFIRST);
  hp->setDataMode(SPI_MODE0);
  for (int i = 0; i < nrf24_count; i++)
  {
    radios[i]->begin(hp);
    radios[i]->setAutoAck(false);
    radios[i]->stopListening();
    radios[i]->setRetries(0, 0);
    radios[i]->setPayloadSize(5);
    radios[i]->setAddressWidth(3);
    int nrf_pa_level;
    switch (nrf_pa){
      case 0:
        nrf_pa_level = 3; break;
      case 1:
        nrf_pa_level = 2; break;
      case 2:
        nrf_pa_level = 1; break;
      case 3:
        nrf_pa_level = 0; break;
    }
    radios[i]->setPALevel(nrf_pa_level, true);
    radios[i]->setDataRate(RF24_2MBPS);
    radios[i]->setCRCLength(RF24_CRC_DISABLED);
  }
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
  InitRadios();
  HSPI_init();
  for (int j = 0; j < nrf24_count; j++)
  {
    radios[j]->startConstCarrier(RF24_PA_MAX, 45);
  }
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    if (Separate_or_together == 0)
    {
      if (bluetooth_jam_method == 0)
      {
        int total_channels = 21;
        int base = total_channels / nrf24_count;
        int rem = total_channels % nrf24_count;
        int ch = 0;
        for (int j = 0; j < nrf24_count; j++) {
          int count = base + (j < rem ? 1 : 0);
          for (int i = 0; i < count; i++, ch++) {
            radios[j]->setChannel(bluetooth_channels[ch]);
          }
        }
      }
      if (bluetooth_jam_method == 1)
      {
        int random_channel = random(80);
        for (int j = 0; j < nrf24_count; j++)
        {
          radios[j]->setChannel(random_channel);
        }
      }
      if (bluetooth_jam_method == 2)
      {
        int total_channels = 80;
        int base = total_channels / nrf24_count;
        int rem = total_channels % nrf24_count;
        int ch = 0;
        for (int j = 0; j < nrf24_count; j++) {
          int count = base + (j < rem ? 1 : 0);
          for (int i = 0; i < count; i++, ch++) {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
    else
    {
      if (bluetooth_jam_method == 0)
      {
        for (int ch = 0; ch < 21; ch++)
        {
          for (int j = 0; j < nrf24_count; j++)
          {
            radios[j]->setChannel(bluetooth_channels[ch]);
          }
        }
      }
      if (bluetooth_jam_method == 1)
      {
        int random_channel = random(80);
        for (int j = 0; j < nrf24_count; j++)
        {
          radios[j]->setChannel(random_channel);
        }
      }
      if (bluetooth_jam_method == 2)
      {
        for (int ch = 0; ch < 80; ch++)
        {
          for (int j = 0; j < nrf24_count; j++)
          {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
  }
  DeinitRadios(true);
  HSPI_deinit();
}
void drone_jam()
{
  InitRadios();
  HSPI_init();
  for (int j = 0; j < nrf24_count; j++)
  {
    radios[j]->startConstCarrier(RF24_PA_MAX, 45);
  }
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    if (Separate_or_together == 0)
    {
      if (drone_jam_method == 0)
      {
        int random_channel = random(125);
        for (int j = 0; j < nrf24_count; j++)
        {
          radios[j]->setChannel(random_channel);
        }
      }
      if (drone_jam_method == 1)
      {
        int total_channels = 125;
        int base = total_channels / nrf24_count;
        int rem = total_channels % nrf24_count;
        int ch = 0;
        for (int j = 0; j < nrf24_count; j++) {
          int count = base + (j < rem ? 1 : 0);
          for (int i = 0; i < count; i++, ch++) {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
    else
    {
      if (drone_jam_method == 0)
      {
        int random_channel = random(125);
        for (int j = 0; j < nrf24_count; j++)
        {
          radios[j]->setChannel(random_channel);
        }
      }
      if (drone_jam_method == 1)
      {
        for (int ch = 0; ch < 125; ch++)
        {
          for (int j = 0; j < nrf24_count; j++)
          {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
  }
  DeinitRadios(true);
  HSPI_deinit();
}
void ble_jam()
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    if (Separate_or_together == 0)
    {
      int total_channels = 3;
      int base = total_channels / nrf24_count;
      int rem = total_channels % nrf24_count;
      int ch = 0;
      for (int j = 0; j < nrf24_count; j++) {
        int count = base + (j < rem ? 1 : 0);
        for (int i = 0; i < count; i++, ch++) {
          radios[j]->setChannel(ble_channels[ch]);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
    else
    {
      for (int ch = 0; ch < 3; ch++)
      {
        for (int j = 0; j < nrf24_count; j++)
        {
          radios[j]->setChannel(ble_channels[ch]);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
}
void wifi_jam()
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    for (int channel = 0; channel < 13; channel++)
    {
      if (Separate_or_together == 0)
      {
        int total_channels = 22;
        int base = total_channels / nrf24_count;
        int rem = total_channels % nrf24_count;
        int ch = (channel * 5) + 1;
        for (int j = 0; j < nrf24_count; j++) {
          int count = base + (j < rem ? 1 : 0);
          for (int i = 0; i < count; i++, ch++) {
            butt1.tick();
            SerialStop = SerialCommands();
            if (butt1.isSingle() || !SerialStop)
              break;
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      else
      {
        for (int ch = (channel * 5) + 1; ch < (channel * 5) + 23; ch++)
        {
          for (int j = 0; j < nrf24_count; j++)
          {
            butt1.tick();
            SerialStop = SerialCommands();
            if (butt1.isSingle() || !SerialStop)
              break;
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      if (butt1.isSingle() || !SerialStop)
        break;
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
}
void wifi_channel(int channel)
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    if (Separate_or_together == 0)
    {
      int total_channels = 22;
      int base = total_channels / nrf24_count;
      int rem = total_channels % nrf24_count;
      int ch = (channel * 5) + 1;
      for (int j = 0; j < nrf24_count; j++) {
        int count = base + (j < rem ? 1 : 0);
        for (int i = 0; i < count; i++, ch++) {
          butt1.tick();
          SerialStop = SerialCommands();
          if (butt1.isSingle() || !SerialStop)
            break;
          radios[j]->setChannel(ch);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
    else
    {
      for (int ch = (channel * 5) + 1; ch < (channel * 5) + 23; ch++)
      {
        for (int j = 0; j < nrf24_count; j++)
        {
          butt1.tick();
          SerialStop = SerialCommands();
          if (butt1.isSingle() || !SerialStop)
            break;
          radios[j]->setChannel(ch);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
}
void zigbee_jam()
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    for (int channel = 11; channel < 27; channel++)
    {
      if (Separate_or_together == 0)
      {
        int total_channels = 3;
        int base = total_channels / nrf24_count;
        int rem = total_channels % nrf24_count;
        int ch = 4 + 5 * (channel - 11);
        for (int j = 0; j < nrf24_count; j++) {
          int count = base + (j < rem ? 1 : 0);
          for (int i = 0; i < count; i++, ch++) {
            butt1.tick();
            SerialStop = SerialCommands();
            if (butt1.isSingle() || !SerialStop)
              break;
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      else
      {
        for (int ch = 5 + 5 * (channel - 11); ch < (5 + 5 * (channel - 11)) + 6; ch++)
        {
          for (int j = 0; j < nrf24_count; j++)
          {
            butt1.tick();
            SerialStop = SerialCommands();
            if (butt1.isSingle() || !SerialStop)
              break;
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      if (butt1.isSingle() || !SerialStop)
        break;
    }
  }
  DeinitRadios(false);
  HSPI_deinit();
}
void misc_jam(int channel1, int channel2)
{
  InitRadios();
  HSPI_init();
  if (misc_jam_method != 1){
    for (int j = 0; j < nrf24_count; j++)
    {
      radios[j]->startConstCarrier(RF24_PA_MAX, 45);
    }
  }
  butt1.tick();
  bool SerialStop = true;
  while (!butt1.isSingle() && SerialStop)
  {
    butt1.tick();
    SerialStop = SerialCommands();
    if (Separate_or_together == 0)
    {
      int total_channels = channel2 - channel1 + 1;
      int base = total_channels / nrf24_count;
      int rem = total_channels % nrf24_count;
      int ch = channel1;
      for (int j = 0; j < nrf24_count; j++) {
        int count = base + (j < rem ? 1 : 0);
        for (int i = 0; i < count; i++, ch++) {
          radios[j]->setChannel(ch);
          if (misc_jam_method == 1)
          {
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
    else
    {
      for (int ch = channel1; ch <= channel2; ch++)
      {
        for (int j = 0; j < nrf24_count; j++)
        {
          radios[j]->setChannel(ch);
          if (misc_jam_method == 1)
          {
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
  }
  if (misc_jam_method != 1)
  {
    DeinitRadios(true);
  }
  else
  {
    DeinitRadios(false);
  }
  HSPI_deinit();
}