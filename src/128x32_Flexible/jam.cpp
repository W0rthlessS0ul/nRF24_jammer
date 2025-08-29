#include "jam.h"

void InitRadios()
{
  for (int i = 0; i < nrf24_count; i++)
  {
    radios[i] = new RF24(ce_pins[i], csn_pins[i]);
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
    radios[i]->setPALevel(RF24_PA_MAX, true);
    radios[i]->setDataRate(RF24_2MBPS);
    radios[i]->setCRCLength(RF24_CRC_DISABLED);
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
  while (!butt1.isSingle())
  {
    butt1.tick();
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
  for (int j = 0; j < nrf24_count; j++)
  {
    radios[j]->stopConstCarrier();
  }
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
  while (!butt1.isSingle())
  {
    butt1.tick();
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
  for (int j = 0; j < nrf24_count; j++)
  {
    radios[j]->stopConstCarrier();
  }
}
void ble_jam()
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  while (!butt1.isSingle())
  {
    butt1.tick();
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
}
void wifi_jam()
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  while (!butt1.isSingle())
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
            if (butt1.isSingle())
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
            if (butt1.isSingle())
              break;
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      if (butt1.isSingle())
        break;
    }
  }
}
void wifi_channel(int channel)
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  while (!butt1.isSingle())
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
          if (butt1.isSingle())
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
          if (butt1.isSingle())
            break;
          radios[j]->setChannel(ch);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
  }
}
void zigbee_jam()
{
  InitRadios();
  HSPI_init();
  butt1.tick();
  while (!butt1.isSingle())
  {
    for (int channel = 11; channel < 27; channel++)
    {
      if (Separate_or_together == 0)
      {
        int total_channels = 6;
        int base = total_channels / nrf24_count;
        int rem = total_channels % nrf24_count;
        int ch = 5 + 5 * (channel - 11);
        for (int j = 0; j < nrf24_count; j++) {
          int count = base + (j < rem ? 1 : 0);
          for (int i = 0; i < count; i++, ch++) {
            butt1.tick();
            if (butt1.isSingle())
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
            if (butt1.isSingle())
              break;
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      if (butt1.isSingle())
        break;
    }
  }
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
  while (!butt1.isSingle())
  {
    butt1.tick();
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
    for (int j = 0; j < nrf24_count; j++)
    {
      radios[j]->stopConstCarrier();
    }
  }
}