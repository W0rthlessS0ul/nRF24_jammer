#include "scan.h"

int scan_wifi_APs(int* channels, bool mode) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int channelCount = 0;
  int networks = WiFi.scanNetworks();
  String current_ssid = getSSIDFromEEPROM();
  String current_password = getPasswordFromEEPROM();

  if (mode){
    memset(channels, 0, 13 * sizeof(int));

    for (int channel = 1; channel <= 14; channel++) {
      bool channelHasNetworks = false;
      
      for (int i = 0; i < networks; i++) {
        if (WiFi.channel(i) == channel) {
          channelHasNetworks = true;
          break;
        }
      }
      
      if (channelHasNetworks) {
        channels[channelCount] = channel;
        channelCount++;
      }
    }

    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
    return channelCount;
  }
  else {
    memset(channels, 0, 13 * sizeof(int));
    std::fill(std::begin(APs_array), std::end(APs_array), "");


    for (int channel = 1; channel <= 14; channel++) {
      bool channelHasNetworks = false;
      int temp = 0;
      for (int i = 0; i < networks; i++) {
        if (WiFi.channel(i) == channel) {
          temp++;
          APs_array[channel-1] = APs_array[channel-1]+WiFi.SSID(i)+'\n';
          channelHasNetworks = true;
        }
      }
      channels[channel-1] = temp;
      if (channelHasNetworks) 
        channelCount++;
    }
    
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
    return networks;
  }
}