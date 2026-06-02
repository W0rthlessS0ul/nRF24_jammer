// scan.cpp - identico all'originale, incluso qui per completezza del target
// Nessuna modifica necessaria: usa solo WiFi API standard ESP32
#include "scan.h"

// Questo file e' una copia simbolica: includere il sorgente originale
// o compilare direttamente scan.cpp dalla cartella 128x64_Flexible.
// PlatformIO con build_src_filter gestisce l'inclusione automatica.
// FILE INTENTIONALLY MINIMAL - il codice scan e' nel sorgente originale.
// Per usarlo, aggiungere a platformio.ini:
//   build_src_filter = -<*> +<128x64_LOLIN32/> +<128x64_Flexible/scan.cpp>
// oppure copiare il contenuto di scan.cpp qui sotto.

// --- copia di src/128x64_Flexible/scan.cpp ---
#include "WiFi.h"
#include "esp_wifi.h"

int scan_wifi_APs(int *WiFiScanChannels, bool channelsOnly)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks(false, true);
  for ( int i = 0; i < 14; i++ )
    WiFiScanChannels[i] = 0;
  for ( int i = 0; i < 14; i++ )
    APs_array[i] = "";

  int channelCount = 0;
  for ( int i = 0; i < n; i++ )
  {
    int ch = WiFi.channel(i);
    if ( ch >= 1 && ch <= 14 )
    {
      WiFiScanChannels[ch - 1]++;
      APs_array[ch - 1] += WiFi.SSID(i) + "\n";
    }
  }

  if ( channelsOnly )
  {
    int activeChannels[14];
    channelCount = 0;
    for ( int i = 0; i < 14; i++ )
    {
      if ( WiFiScanChannels[i] > 0 )
        activeChannels[channelCount++] = i + 1;
    }
    for ( int i = 0; i < channelCount; i++ )
      WiFiScanChannels[i] = activeChannels[i];
    return channelCount;
  }
  return n;
}
