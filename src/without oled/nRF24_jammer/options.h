//  Changeable parameters  
RF24 radio(16, 15, 16000000);
RF24 radio1(22, 21, 16000000);
const char *ssid = "jammer";
const char *password = "W0rthlessS0ul";

//  Unchangeable parameters
#define EEPROM_SIZE 512
SPIClass *sp = nullptr;
SPIClass *hp = nullptr;
int bluetooth_jam_method;
int drone_jam_method;
int ble_jam_method;
int wifi_jam_method;
int zigbee_jam_method;
int Separate_or_together;
byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
byte bluetooth_channels_reversed[] = {80, 78, 76, 74, 30, 28, 26, 24, 22, 8, 6, 4, 2, 1, 0, 52, 50, 48, 46, 34, 32};
byte ble_channels[] = {2, 26, 80};
byte ble_channels_reversed[] = {80, 26, 2};
const char jam_text[] = "xxxxxxxxxxxxxxxx";
WebServer server(80); 