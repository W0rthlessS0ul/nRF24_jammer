#include "options.h"

//  Changeable parameters
RF24 radio(16, 15, 16000000);
GButton butt1(25);
GButton buttNext(26);
GButton buttPrevious(27);
const char *default_ssid = "jammer";
const char *default_password = "W0rthlessS0ul";

//  Unchangeable parameters
uint8_t SCREEN_WIDTH = 128;
uint8_t SCREEN_HEIGHT = 32;
int8_t OLED_RESET = -1;
size_t EEPROM_SIZE = 512;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SPIClass *hp = nullptr;
int menu_number = 0;
int bluetooth_jam_method;
int drone_jam_method;
int ble_jam_method;
int wifi_jam_method;
int zigbee_jam_method;
int misc_jam_method;
int logo;
int access_point;
int buttons;
int channel1 = 0;
int channel2 = 0;
int flag = 0;
bool hspi = false;
byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
byte ble_channels[] = {2, 26, 80};
const char jam_text[] = "xxxxxxxxxxxxxxxx";
WebServer server(80);