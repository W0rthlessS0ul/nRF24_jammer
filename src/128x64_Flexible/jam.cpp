#include "jam.h"

int        nrf_pa_level;
static int global_ch1, global_ch2;
bool       stopJam = false;

void InitRadios()
{
  for ( int i = 0; i < nrf24_count; i++ )
  {
    radios[i] = new RF24(ce_pins[i], csn_pins[i]);
  }
}
void DeinitRadios(bool stopConstCarrier)
{
  for ( int i = 0; i < nrf24_count; i++ )
  {
    if ( radios[i] != nullptr )
    {
      if ( stopConstCarrier )
      {
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
  for ( int i = 0; i < nrf24_count; i++ )
  {
    radios[i]->begin(hp);
    radios[i]->setAutoAck(false);
    radios[i]->stopListening();
    radios[i]->setRetries(0, 0);
    radios[i]->setPayloadSize(5);
    radios[i]->setAddressWidth(3);
    int nrf_pa_level;
    switch ( nrf_pa )
    {
    case 0:
      nrf_pa_level = RF24_PA_MAX;
      break;
    case 1:
      nrf_pa_level = RF24_PA_HIGH;
      break;
    case 2:
      nrf_pa_level = RF24_PA_LOW;
      break;
    case 3:
      nrf_pa_level = RF24_PA_MIN;
      break;
    }
    radios[i]->setPALevel(nrf_pa_level, true);
    radios[i]->setDataRate(RF24_2MBPS);
    radios[i]->setCRCLength(RF24_CRC_DISABLED);
    radios[i]->disableCRC();
    radios[i]->disableAckPayload();
    radios[i]->disableDynamicPayloads();
  }
  digitalWrite(2, HIGH);
}
void HSPI_deinit()
{
  if ( hp != nullptr )
  {
    hp->end();
    delete hp;
    hp = nullptr;
  }
  digitalWrite(2, LOW);
}
void bluetooth_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    if ( Separate_or_together == 0 )
    {
      if ( bluetooth_jam_method == 0 )
      {
        int total_channels = 21;
        int base           = total_channels / nrf24_count;
        int rem            = total_channels % nrf24_count;
        int ch             = 0;
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int count = base + (j < rem ? 1 : 0);
          for ( int i = 0; i < count; i++, ch++ )
          {
            radios[j]->setChannel(bluetooth_channels[ch]);
          }
        }
      }
      if ( bluetooth_jam_method == 1 )
      {
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int random_channel = random(80);
          radios[j]->setChannel(random_channel);
        }
      }
      if ( bluetooth_jam_method == 2 )
      {
        int total_channels = 80;
        int base           = total_channels / nrf24_count;
        int rem            = total_channels % nrf24_count;
        int ch             = 0;
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int count = base + (j < rem ? 1 : 0);
          for ( int i = 0; i < count; i++, ch++ )
          {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
    else
    {
      if ( bluetooth_jam_method == 0 )
      {
        for ( int ch = 0; ch < 21; ch++ )
        {
          for ( int j = 0; j < nrf24_count; j++ )
          {
            radios[j]->setChannel(bluetooth_channels[ch]);
          }
        }
      }
      if ( bluetooth_jam_method == 1 )
      {
        int random_channel = random(80);
        for ( int j = 0; j < nrf24_count; j++ )
        {
          radios[j]->setChannel(random_channel);
        }
      }
      if ( bluetooth_jam_method == 2 )
      {
        for ( int ch = 0; ch < 80; ch++ )
        {
          for ( int j = 0; j < nrf24_count; j++ )
          {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void drone_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    if ( Separate_or_together == 0 )
    {
      if ( drone_jam_method == 0 )
      {
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int random_channel = random(125);
          radios[j]->setChannel(random_channel);
        }
      }
      if ( drone_jam_method == 1 )
      {
        int total_channels = 125;
        int base           = total_channels / nrf24_count;
        int rem            = total_channels % nrf24_count;
        int ch             = 0;
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int count = base + (j < rem ? 1 : 0);
          for ( int i = 0; i < count; i++, ch++ )
          {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
    else
    {
      if ( drone_jam_method == 0 )
      {
        int random_channel = random(125);
        for ( int j = 0; j < nrf24_count; j++ )
        {
          radios[j]->setChannel(random_channel);
        }
      }
      if ( drone_jam_method == 1 )
      {
        for ( int ch = 0; ch < 125; ch++ )
        {
          for ( int j = 0; j < nrf24_count; j++ )
          {
            radios[j]->setChannel(ch);
          }
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void ble_advertising_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    if ( Separate_or_together == 0 )
    {
      int total_channels = 3;
      int base           = total_channels / nrf24_count;
      int rem            = total_channels % nrf24_count;
      int ch             = 0;
      for ( int j = 0; j < nrf24_count; j++ )
      {
        int count = base + (j < rem ? 1 : 0);
        for ( int i = 0; i < count; i++, ch++ )
        {
          radios[j]->setChannel(ble_channels[ch]);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
    else
    {
      for ( int ch = 0; ch < 3; ch++ )
      {
        for ( int j = 0; j < nrf24_count; j++ )
        {
          radios[j]->setChannel(ble_channels[ch]);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void ble_data_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    if ( Separate_or_together == 0 )
    {
      int total_channels = 40;
      int base           = total_channels / nrf24_count;
      int rem            = total_channels % nrf24_count;
      int ch             = 2;
      for ( int j = 0; j < nrf24_count; j++ )
      {
        int count = base + (j < rem ? 1 : 0);
        for ( int i = 0; i < count; i++, ch += 2 )
        {
          radios[j]->setChannel(ch);
        }
      }
    }
    else
    {
      for ( int ch = 2; ch <= 80; ch += 2 )
      {
        for ( int j = 0; j < nrf24_count; j++ )
        {
          radios[j]->setChannel(ch);
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void wifi_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    for ( int channel = 0; channel < 14; channel++ )
    {
      if ( Separate_or_together == 0 )
      {
        int total_channels = 23;
        int base           = total_channels / nrf24_count;
        int rem            = total_channels % nrf24_count;
        int ch             = (channel * 5) + 1;
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int count = base + (j < rem ? 1 : 0);
          for ( int i = 0; i < count; i++, ch++ )
          {
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      else
      {
        for ( int ch = (channel * 5) + 1; ch <= (channel * 5) + 23; ch++ )
        {
          for ( int j = 0; j < nrf24_count; j++ )
          {
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void wifi_scan_jam()
{
  vTaskPrioritySet(NULL, 3);
  int scanCounter = 0;
  while ( !stopJam )
  {
    if ( scanCounter >= 10000 )
    {
      NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
      scanCounter    = 0;
    }

    for ( int chIndex = 0; chIndex < NumberChannels; chIndex++ )
    {
      int wifiChannel = WiFiScanChannels[chIndex];

      if ( Separate_or_together == 0 )
      {
        int total_channels = 23;
        int base           = total_channels / nrf24_count;
        int rem            = total_channels % nrf24_count;
        int ch             = ((wifiChannel - 1) * 5) + 1;

        for ( int j = 0; j < nrf24_count; j++ )
        {
          int count = base + (j < rem ? 1 : 0);
          for ( int i = 0; i < count; i++, ch++ )
          {
            if ( ch >= 1 && ch <= 125 )
            {
              radios[j]->setChannel(ch);
              radios[j]->writeFast(&jam_text, sizeof(jam_text));
            }
          }
        }
      }
      else
      {
        for ( int ch = ((wifiChannel - 1) * 5) + 1; ch <= ((wifiChannel - 1) * 5) + 23; ch++ )
        {
          for ( int j = 0; j < nrf24_count; j++ )
          {
            if ( ch >= 1 && ch <= 125 )
            {
              radios[j]->setChannel(ch);
              radios[j]->writeFast(&jam_text, sizeof(jam_text));
            }
          }
        }
      }
    }
    scanCounter++;
  }
  vTaskDelete(NULL);
}
void wifi_channel()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    if ( Separate_or_together == 0 )
    {
      int total_channels = 23;
      int base           = total_channels / nrf24_count;
      int rem            = total_channels % nrf24_count;
      int ch             = (global_ch1 * 5) + 1;
      for ( int j = 0; j < nrf24_count; j++ )
      {
        int count = base + (j < rem ? 1 : 0);
        for ( int i = 0; i < count; i++, ch++ )
        {
          radios[j]->setChannel(ch);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
    else
    {
      for ( int ch = (global_ch1 * 5) + 1; ch <= (global_ch1 * 5) + 23; ch++ )
      {
        for ( int j = 0; j < nrf24_count; j++ )
        {
          radios[j]->setChannel(ch);
          radios[j]->writeFast(&jam_text, sizeof(jam_text));
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void zigbee_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    for ( int channel = 11; channel < 27; channel++ )
    {
      if ( Separate_or_together == 0 )
      {
        int total_channels = 3;
        int base           = total_channels / nrf24_count;
        int rem            = total_channels % nrf24_count;
        int ch             = 4 + 5 * (channel - 11);
        for ( int j = 0; j < nrf24_count; j++ )
        {
          int count = base + (j < rem ? 1 : 0);
          for ( int i = 0; i < count; i++, ch++ )
          {
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
      else
      {
        for ( int ch = 4 + 5 * (channel - 11); ch <= (5 + 5 * (channel - 11)) + 2; ch++ )
        {
          for ( int j = 0; j < nrf24_count; j++ )
          {
            radios[j]->setChannel(ch);
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
  }
  vTaskDelete(NULL);
}
void misc_jam()
{
  vTaskPrioritySet(NULL, 3);
  while ( !stopJam )
  {
    if ( Separate_or_together == 0 )
    {
      int total_channels = global_ch2 - global_ch1 + 1;
      int base           = total_channels / nrf24_count;
      int rem            = total_channels % nrf24_count;
      int ch             = global_ch1;
      for ( int j = 0; j < nrf24_count; j++ )
      {
        int count = base + (j < rem ? 1 : 0);
        for ( int i = 0; i < count; i++, ch++ )
        {
          radios[j]->setChannel(ch);
          if ( misc_jam_method == 1 )
          {
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
    else
    {
      for ( int ch = global_ch1; ch <= global_ch2; ch++ )
      {
        for ( int j = 0; j < nrf24_count; j++ )
        {
          radios[j]->setChannel(ch);
          if ( misc_jam_method == 1 )
          {
            radios[j]->writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
  }
  vTaskDelete(NULL);
}

void jamHandler(void (*action)(), String htmlResponse, const unsigned char *bitmap, bool CW, bool mode, bool isMisc, bool scan, int ch1, int ch2)
{
  global_ch1              = ch1;
  global_ch2              = ch2;
  stopJam                 = false;
  String       html       = FPSTR(html_jam);
  TaskHandle_t TaskHandle = NULL;

  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();

  html.replace("[||]EdItAbLe TeXt[||]", htmlResponse);
  server.send(200, "text/html", html.c_str());
  html.replace(htmlResponse, "[||]EdItAbLe TeXt[||]");

  if ( scan )
  {
    delay(500);
    if ( access_point == 0 )
      WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    delay(100);
    NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
  }

  InitRadios();
  HSPI_init();
  if ( isMisc )
  {
    if ( misc_jam_method != 1 )
    {
      for ( int j = 0; j < nrf24_count; j++ )
      {
        radios[j]->startConstCarrier(static_cast<rf24_pa_dbm_e>(nrf_pa_level), 45);
      }
    }
  }
  else
  {
    if ( CW )
    {
      for ( int j = 0; j < nrf24_count; j++ )
      {
        radios[j]->startConstCarrier(static_cast<rf24_pa_dbm_e>(nrf_pa_level), 45);
      }
    }
  }

  xTaskCreatePinnedToCore(reinterpret_cast<TaskFunction_t>(action), "Jammer", 4096, NULL, 3, &TaskHandle, 0);

  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  bool SerialStop = true;

  while ( !btnOK.isSingle() && SerialStop )
  {
    randomSeed(esp_random() + millis());
    btnNext.tick();
    btnOK.tick();
    btnPrevious.tick();
    SerialStop = SerialCommands();
    if ( (btnNext.isSingle() || btnPrevious.isSingle() || btnOK.isDouble()) && mode )
      break;
    delay(1);
  }

  stopJam = true;
  while ( eTaskGetState(TaskHandle) != eDeleted )
    delay(1);

  TaskHandle = NULL;

  if ( isMisc )
  {
    if ( misc_jam_method != 1 )
      DeinitRadios(true);
    else
      DeinitRadios(false);
  }
  else
  {
    if ( CW )
      DeinitRadios(true);
    else
      DeinitRadios(false);
  }

  HSPI_deinit();
  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  updateDisplay(menu_number);
}