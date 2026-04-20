#include "scan.h"

int scan_wifi_APs(int *channels, bool mode)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int    channelCount     = 0;
  int    networks         = WiFi.scanNetworks();
  String current_ssid     = getSSIDFromEEPROM();
  String current_password = getPasswordFromEEPROM();

  if ( mode )
  {
    memset(channels, 0, 13 * sizeof(int));

    for ( int channel = 1; channel <= 14; channel++ )
    {
      bool channelHasNetworks = false;

      for ( int i = 0; i < networks; i++ )
      {
        if ( WiFi.channel(i) == channel )
        {
          channelHasNetworks = true;
          break;
        }
      }

      if ( channelHasNetworks )
      {
        channels[channelCount] = channel;
        channelCount++;
      }
    }

    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
    return channelCount;
  }
  else
  {
    memset(channels, 0, 13 * sizeof(int));
    std::fill(std::begin(APs_array), std::end(APs_array), "");

    for ( int channel = 1; channel <= 14; channel++ )
    {
      bool channelHasNetworks = false;
      int  temp               = 0;
      for ( int i = 0; i < networks; i++ )
      {
        if ( WiFi.channel(i) == channel )
        {
          temp++;
          APs_array[channel - 1] = APs_array[channel - 1] + WiFi.SSID(i) + '\n';
          channelHasNetworks     = true;
        }
      }
      channels[channel - 1] = temp;
      if ( channelHasNetworks )
        channelCount++;
    }

    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
    return networks;
  }
}

int scan_BLE()
{
  BLEDevice::init("");
  delay(100);

  for ( int i = 0; i < 100; i++ )
  {
    BLE_MAC[i]              = "";
    BLE_RSSI[i]             = 0;
    BLE_Name[i]             = "";
    BLE_ManufacturerData[i] = "";
    BLE_Payload[i]          = "";
  }

  NimBLEScan *pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  NimBLEScanResults foundDevices = pBLEScan->getResults(5000, false);

  int count = foundDevices.getCount();

  for ( int i = 0; i < count && i < 100; i++ )
  {
    const NimBLEAdvertisedDevice *device = foundDevices.getDevice(i);

    BLE_MAC[i]  = device->getAddress().toString().c_str();
    BLE_RSSI[i] = device->getRSSI();

    if ( device->haveName() )
    {
      BLE_Name[i] = device->getName().c_str();
    }
    else
    {
      BLE_Name[i] = "(unknown)";
    }

    if ( device->haveManufacturerData() )
    {
      std::string mdata    = device->getManufacturerData();
      char        hex[128] = {0};
      for ( size_t j = 0; j < mdata.length() && j < 60; j++ )
      {
        sprintf(hex + strlen(hex), "%02X ", (uint8_t)mdata[j]);
      }
      BLE_ManufacturerData[i] = hex;
    }

    const std::vector<uint8_t> &payloadVec = device->getPayload();
    size_t                      len        = payloadVec.size();

    char hexPayload[512] = {0};
    for ( size_t j = 0; j < len && j < 250; j++ )
    {
      sprintf(hexPayload + strlen(hexPayload), "%02X ", payloadVec[j]);
    }
    BLE_Payload[i] = hexPayload;
  }

  if ( pBLEScan->isScanning() )
    pBLEScan->stop();
  NimBLEDevice::deinit(true);
  esp_bt_controller_disable();
  delay(100);
  esp_bt_controller_deinit();

  return count;
}