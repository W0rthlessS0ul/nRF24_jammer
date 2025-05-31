//  Changeable parameters  
RF24 radio(16, 15, 16000000);
RF24 radio1(22, 21, 16000000);
const char *default_ssid = "jammer";
const char *default_password = "W0rthlessS0ul";

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
int misc_jam_method;
byte bluetooth_channels[] = {2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80};
byte bluetooth_channels_reversed[] = {"80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2"};
byte ble_channels[] = {1, 2, 3, 25, 26, 27, 79, 80, 81};
byte ble_channels_reversed[] = {"81, 80, 79, 27, 26, 25, 3, 2, 1"};
const char jam_text[] = "xxxxxxxxxxxxxxxx";
WebServer server(80); 
