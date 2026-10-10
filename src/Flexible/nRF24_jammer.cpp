#include "bitmap.h"
#include "config.h"
#include "html.h"
#include "jam.h"
#include "scan.h"
#include "serial.h"

static String jsonEscape(const String &s)
{
  String out;
  out.reserve(s.length() + 8);
  for ( size_t i = 0; i < s.length(); i++ )
  {
    char c = s[i];
    switch ( c )
    {
    case '\\':
      out += "\\\\";
      break;
    case '"':
      out += "\\\"";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    default:
      out += c;
      break;
    }
  }
  return out;
}

void handleRoot()
{
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)html, html_len);
}

void HandleWebCommand()
{
  String command = server.arg("command");
  command.trim();
  command.toLowerCase();
  CommandsHandler(command, true);
}

void updateDisplay(int menuNum)
{
  display.clearDisplay();
  const uint8_t *bitmap = (menu_number == 0)   ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_bluetooth_jammer_64 : bitmap_bluetooth_jammer_32)
                          : (menu_number == 1) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_drone_jammer_64 : bitmap_drone_jammer_32)
                          : (menu_number == 2) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_attacks_64 : bitmap_wifi_attacks_32)
                          : (menu_number == 3) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_ble_jammer_64 : bitmap_ble_jammer_32)
                          : (menu_number == 4) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_zigbee_jammer_64 : bitmap_zigbee_jammer_32)
                          : (menu_number == 5) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_misc_jammer_64 : bitmap_misc_jammer_32)
                                               : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_settings_64 : bitmap_settings_32);
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
}

void handlerSetSettings()
{
  baudrate             = server.arg("baudrate").toInt();
  logo                 = server.arg("logo").toInt();
  display_setting      = server.arg("display").toInt();
  SCREEN_HEIGHT        = server.arg("screen_height").toInt();
  buttons              = server.arg("button").toInt();
  Separate_or_together = server.arg("sweep").toInt();
  nrf_pa               = server.arg("pa").toInt();
  bluetooth_jam_method = server.arg("bt_method").toInt();
  drone_jam_method     = server.arg("drone_method").toInt();
  misc_jam_method      = server.arg("misc_method").toInt();
  ssid                 = server.arg("ssid");
  password             = server.arg("password");
  access_point         = server.arg("ap").toInt();
  jam_delay            = server.arg("delay").toInt();

  String beacon_list = server.arg("beacon");
  String ce_list     = server.arg("ce");
  String csn_list    = server.arg("csn");

  auto parseTokens = [](const String &str, auto callback, int maxCount) -> int {
    if ( str.length() == 0 )
      return 0;

    int count = 0;
    int start = 0;
    while ( count < maxCount )
    {
      int sep = str.indexOf('|', start);
      if ( sep == -1 )
        sep = str.length();

      String token = str.substring(start, sep);
      token.trim();

      if ( token.length() > 0 )
      {
        callback(token, count++);
      }

      if ( sep == str.length() )
        break;
      start = sep + 1;
    }
    return count;
  };

  nrf24_count = parseTokens(ce_list, [](const String &token, int idx) { ce_pins[idx] = token.toInt(); }, 30);

  parseTokens(csn_list, [](const String &token, int idx) { csn_pins[idx] = token.toInt(); }, 30);

  memset(SSIDs_Array, 0, sizeof(SSIDs_Array));
  parseTokens(beacon_list, [](const String &token, int idx) {
    String ssid_str = token.length() > 31 ? token.substring(0, 31) : token;
    strlcpy(SSIDs_Array[idx], ssid_str.c_str(), sizeof(SSIDs_Array[idx])); }, 100);

  prefs.putInt("baudrate", baudrate);
  prefs.putInt("logo_configs", logo);
  prefs.putInt("display_configs", display_setting);
  prefs.putInt("screen_height", SCREEN_HEIGHT);
  prefs.putInt("buttons_configs", buttons);
  prefs.putInt("SorT_configs", Separate_or_together);
  prefs.putInt("PA_configs", nrf_pa);
  prefs.putInt("bt_configs", bluetooth_jam_method);
  prefs.putInt("drone_configs", drone_jam_method);
  prefs.putInt("misc_configs", misc_jam_method);
  prefs.putInt("nRF24_count", nrf24_count);
  prefs.putString("ssid", ssid);
  prefs.putString("password", password);
  prefs.putInt("AP_configs", access_point);
  prefs.putInt("jam_delay", jam_delay);

  prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));
  prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));

  Serial.updateBaudRate(baudrate);

  if ( SCREEN_HEIGHT == 32 )
  {
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x02);
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x1F);
  }
  else
  {
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x12);
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x3F);
  }

  server.send(200, "text/plain", "OK");
}

void handlenRF24Init()
{
  nrf24_count = prefs.getInt("nRF24_count", 0);

  prefs.getBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.getBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
}

void handlenRF24Scan()
{
  HSPI_init();

  auto isSafePin = [&](int pin) {
    if ( !GPIO_IS_VALID_OUTPUT_GPIO(pin) )
      return false;
    switch ( pin )
    {
    case 1:
    case 3:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 22:
    case 21:
    case 25:
    case 26:
    case 27:
    case 2:
      return false;
    default:
      return true;
    }
  };
  auto readReg = [&](int csnPin, uint8_t reg) {
    hp->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(csnPin, LOW);
    hp->transfer(0x00 | reg);
    uint8_t val = hp->transfer(0xFF);
    digitalWrite(csnPin, HIGH);
    hp->endTransaction();
    return val;
  };
  auto writeReg = [&](int csnPin, uint8_t reg, uint8_t val) {
    hp->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(csnPin, LOW);
    hp->transfer(0x20 | reg);
    hp->transfer(val);
    digitalWrite(csnPin, HIGH);
    hp->endTransaction();
  };

  for ( int p = 0; p < SOC_GPIO_PIN_COUNT; p++ )
  {
    if ( isSafePin(p) )
    {
      pinMode(p, OUTPUT);
      digitalWrite(p, HIGH);
    }
  }

  int csn_pins_count = 0;
  for ( int csn = 0; csn < SOC_GPIO_PIN_COUNT; csn++ )
  {
    if ( !isSafePin(csn) )
      continue;

    writeReg(csn, 0x05, 0x33);
    uint8_t test = readReg(csn, 0x05);
    if ( test != 0x33 )
      continue;

    writeReg(csn, 0x05, 0x44);
    test = readReg(csn, 0x05);
    if ( test != 0x44 )
      continue;

    csn_pins[csn_pins_count] = csn;
    csn_pins_count++;
  }

  for ( int p = 0; p < SOC_GPIO_PIN_COUNT; p++ )
  {
    if ( isSafePin(p) )
    {
      bool is_csn = false;
      for ( int i = 0; i < csn_pins_count; i++ )
      {
        if ( csn_pins[i] == p )
        {
          is_csn = true;
          break;
        }
      }
      if ( !is_csn )
      {
        digitalWrite(p, LOW);
      }
    }
  }

  if ( csn_pins_count == 0 )
  {
    HSPI_deinit();
    return;
  }

  int ce_pins_count = 0;
  for ( int ce = 0; ce < SOC_GPIO_PIN_COUNT; ce++ )
  {
    if ( !isSafePin(ce) )
      continue;

    auto csn = std::find(std::begin(csn_pins), std::end(csn_pins), ce);
    if ( csn != std::end(csn_pins) )
      continue;

    for ( int i = 0; i < csn_pins_count; i++ )
    {
      int csn_pin = csn_pins[i];

      writeReg(csn_pin, 0x01, 0x00);
      writeReg(csn_pin, 0x00, 0x0E);
      delay(5);

      writeReg(csn_pin, 0x07, 0x70);

      hp->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
      digitalWrite(csn_pin, LOW);
      hp->transfer(0xE1);
      digitalWrite(csn_pin, HIGH);

      digitalWrite(csn_pin, LOW);
      hp->transfer(0xA0);
      hp->transfer(0x55);
      digitalWrite(csn_pin, HIGH);
      hp->endTransaction();

      digitalWrite(ce, HIGH);
      delayMicroseconds(50);
      digitalWrite(ce, LOW);

      delay(5);

      uint8_t status = readReg(csn_pin, 0x07);
      if ( (status & 0x20) == 0 )
        continue;

      ce_pins[ce_pins_count] = ce;
      ce_pins_count++;
    }
  }

  if ( ce_pins_count != csn_pins_count )
  {
    HSPI_deinit();
    return;
  }

  nrf24_count = ce_pins_count;

  prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
  prefs.putInt("nRF24_count", nrf24_count);

  HSPI_deinit();
}

void RescanHandler()
{
  scan_wifi_APs(WiFiScanChannels, false);
  handleRoot();
}

void sendHtmlAndExecute(const uint8_t *htmlData, size_t len, void (*action)() = nullptr)
{
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)htmlData, len);
  delay(500);
  if ( action )
    action();
}

void attackHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap)
{
  (void)htmlResponse;
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  sendHtmlAndExecute(html_jam, html_jam_len, attackFunction);
  updateDisplay(menu_number);
}

void attackScanHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap)
{
  (void)htmlResponse;
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)html_jam, html_jam_len);
  delay(500);
  if ( access_point == 0 )
    WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);
  delay(100);
  NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
  attackFunction();
  updateDisplay(menu_number);
}

void miscChannelsHandler()
{
  int channel1 = server.arg("start").toInt();
  int channel2 = server.arg("stop").toInt();

  sendHtmlAndExecute(html_jam, html_jam_len);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Start");
  display.setCursor(96, 0);
  display.println("Stop");
  display.setCursor(52, 0);
  display.println("Width");
  display.setCursor(8, 10);
  display.println(String(channel1));
  display.setCursor(60, 10);
  display.println(String(channel2 - channel1));
  display.setCursor(100, 10);
  display.println(String(channel2));
  display.setCursor(0, 20);
  display.println("Jamming Started");
  display.display();

  jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_misc_jammer_64 : bitmap_misc_jammer_32), false, false, true, false, channel1, channel2);
  updateDisplay(menu_number);
}

void wifiChannelsHandler()
{
  int channel = server.arg("channel").toInt();

  display.clearDisplay();
  display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  sendHtmlAndExecute(html_jam, html_jam_len);

  jamHandler(wifi_channel, "Jamming WiFi channel", decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, false, false, false, channel - 1, 0);
  updateDisplay(menu_number);
}

void wifiDeauthChannelsHandler()
{
  int channel = server.arg("channel").toInt();

  display.clearDisplay();
  display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  sendHtmlAndExecute(html_jam, html_jam_len);

  wifi_deauth_channel(channel);
  updateDisplay(menu_number);
}

void handleApiStatus()
{
  String json;
  json.reserve(1024);

  json += "{\"nrf24_count\":" + String(nrf24_count) + ",";

  String btText;
  switch ( bluetooth_jam_method )
  {
  case 0:
    btText = "21 channels";
    break;
  case 1:
  case 2:
    btText = "80 channels";
    break;
  default:
    btText = "-";
  }
  json += "\"bt_channels_text\":\"" + btText + "\",";
  json += "\"wifi_jam_method\":" + String(wifi_jam_method) + ",";

  json += "\"ssids_by_channel\":[";
  for ( int ch = 0; ch < 14; ch++ )
  {
    json += "\"" + jsonEscape(String(APs_array[ch])) + "\"";
    if ( ch < 13 )
      json += ",";
  }
  json += "],";

  json += "\"wifi_channels\":[";
  for ( int ch = 0; ch < 14; ch++ )
  {
    json += String(WiFiScanChannels[ch]);
    if ( ch < 13 )
      json += ",";
  }
  json += "]";

  json += "}";
  server.send(200, "application/json", json);
}

void handleApiSettings()
{
  String json;
  json.reserve(2048);

  json += "{\"version\":\"" + String(Version_Number) + "\",";
  json += "\"baudrate\":" + String(baudrate) + ",";
  json += "\"logo\":" + String(logo) + ",";
  json += "\"display\":" + String(display_setting) + ",";
  json += "\"screen_height\":" + String(SCREEN_HEIGHT) + ",";
  json += "\"button\":" + String(buttons) + ",";
  json += "\"sweep\":" + String(Separate_or_together) + ",";
  json += "\"pa\":" + String(nrf_pa) + ",";
  json += "\"bt\":" + String(bluetooth_jam_method) + ",";
  json += "\"drone\":" + String(drone_jam_method) + ",";
  json += "\"misc\":" + String(misc_jam_method) + ",";
  json += "\"delay\":" + String(jam_delay) + ",";
  json += "\"ap\":" + String(access_point) + ",";
  json += "\"nrf24_count\":" + String(nrf24_count) + ",";

  json += "\"ssid\":\"" + jsonEscape(ssid) + "\",";
  json += "\"password\":\"" + jsonEscape(password) + "\",";

  json += "\"beacon\":[";
  bool firstB = true;
  for ( int i = 0; i < 100; i++ )
  {
    if ( SSIDs_Array[i][0] == '\0' )
      break;
    if ( !firstB )
      json += ",";
    json += "\"" + jsonEscape(String(SSIDs_Array[i])) + "\"";
    firstB = false;
  }
  json += "],";

  json += "\"ce_pins\":[";
  for ( int i = 0; i < nrf24_count; i++ )
  {
    json += String(ce_pins[i]);
    if ( i < nrf24_count - 1 )
      json += ",";
  }
  json += "],";

  json += "\"csn_pins\":[";
  for ( int i = 0; i < nrf24_count; i++ )
  {
    json += String(csn_pins[i]);
    if ( i < nrf24_count - 1 )
      json += ",";
  }
  json += "]";

  json += "}";
  server.send(200, "application/json", json);
}

void handleApiVersion()
{
  String json = "{\"version\":\"" + String(Version_Number) + "\"}";
  server.send(200, "application/json", json);
}

void storeEEPROMAndSet(const char *index, int value, int &targetVar)
{
  prefs.putInt(index, value);
  targetVar = value;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void saveWiFiSettings(const char *new_ssid, const char *new_password)
{
  prefs.putString("ssid", new_ssid);

  prefs.putString("password", new_password);
}

void handleResetWiFiSettings()
{
  saveWiFiSettings(default_ssid, default_password);

  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, "text/html", (const char *)html_pls_reboot, html_pls_reboot_len);
  delay(1000);
  ESP.restart();
}

void handleFileUpload()
{
  HTTPUpload          &upload         = server.upload();
  static unsigned long lastUpdate     = 0;
  const unsigned long  updateInterval = 10;

  if ( upload.status == UPLOAD_FILE_START )
  {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if ( !Update.begin(UPDATE_SIZE_UNKNOWN) )
    {
      Update.printError(Serial);
      return;
    }
    else
    {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      lastUpdate = millis();
    }
  }
  else if ( upload.status == UPLOAD_FILE_WRITE )
  {
    if ( Update.write(upload.buf, upload.currentSize) != upload.currentSize )
    {
      Update.printError(Serial);
    }

    if ( millis() - lastUpdate >= updateInterval )
    {
      lastUpdate = millis();
    }
  }
  else if ( upload.status == UPLOAD_FILE_END )
  {
  }
  else if ( upload.status == UPLOAD_FILE_ABORTED )
  {
    Update.abort();
    Serial.println("Update aborted");
  }
}

void settings()
{
  int  current_item  = 0;
  int  scroll_offset = 0;
  bool in_edit_mode  = false;

  auto draw_settings = [&]() {
    int total_items = (buttons == 0) ? 13 : 12;
    if ( current_item >= total_items )
      current_item = total_items - 1;

    display.clearDisplay();

    int max_visible = (SCREEN_HEIGHT == 64) ? 6 : 3;
    int line_height = 10;
    int start_y     = 1;

    if ( current_item < scroll_offset )
    {
      scroll_offset = current_item;
    }
    else if ( current_item >= scroll_offset + max_visible )
    {
      scroll_offset = current_item - max_visible + 1;
    }

    for ( int i = 0; i < max_visible; i++ )
    {
      int item_idx = scroll_offset + i;
      if ( item_idx >= total_items )
        break;

      int y = start_y + i * line_height;

      if ( item_idx == current_item )
      {
        display.fillRect(0, y - 1, 122, line_height - 1, WHITE);
        display.setTextColor(BLACK, WHITE);
      }
      else
      {
        display.setTextColor(WHITE, BLACK);
      }

      display.setCursor(4, y);

      switch ( item_idx )
      {
      case 0:
        display.print("Logo: ");
        display.print(logo == 0 ? "Show" : "Hide");
        break;
      case 1:
        display.print("Display: ");
        display.print(display_setting == 0 ? "On" : "Off");
        break;
      case 2:
        display.print("Height: ");
        display.print(SCREEN_HEIGHT);
        break;
      case 3:
        display.print("Buttons: ");
        display.print(buttons == 0 ? "1 Btn" : (buttons == 1 ? "2 Btn" : "3 Btn"));
        break;
      case 4:
        display.print("Sweep: ");
        display.print(Separate_or_together == 0 ? "Sep" : "Together");
        break;
      case 5:
        display.print("PA Level: ");
        display.print(nrf_pa == 0 ? "MAX" : (nrf_pa == 1 ? "HIGH" : (nrf_pa == 2 ? "LOW" : "MIN")));
        break;
      case 6:
        display.print("BT Mode: ");
        display.print(bluetooth_jam_method == 0 ? "21ch" : (bluetooth_jam_method == 1 ? "80ch rand" : "80ch brute"));
        break;
      case 7:
        display.print("Drone: ");
        display.print(drone_jam_method == 0 ? "Random" : "Brute");
        break;
      case 8:
        display.print("Misc: ");
        display.print(misc_jam_method == 0 ? "CW" : "Packet");
        break;
      case 9:
        display.print("AP Mode: ");
        display.print(access_point == 0 ? "On" : "Off");
        break;
      case 10:
        display.print("Baudrate: ");
        display.print(baudrate);
        break;
      case 11:
        display.print("Jam Delay: ");
        if ( in_edit_mode && current_item == 11 )
        {
          display.print("<");
          display.print(jam_delay);
          display.print("ms>");
        }
        else
        {
          display.print(jam_delay);
          display.print("ms");
        }
        break;
      case 12:
        display.print(" [ Save ]");
        break;
      }
    }

    display.drawFastVLine(125 + 1, 0, SCREEN_HEIGHT, WHITE);
    int thumb_h = (SCREEN_HEIGHT * max_visible) / total_items;
    if ( thumb_h < 4 )
      thumb_h = 4;
    int thumb_y = map(current_item, 0, total_items - 1, 0, SCREEN_HEIGHT - thumb_h);
    display.fillRect(125, thumb_y, 3, thumb_h, WHITE);

    display.display();
  };

  auto apply_and_save_item = [&](int item_idx, int delta) {
    switch ( item_idx )
    {
    case 0:
      logo = (logo + 1) % 2;
      prefs.putInt("logo_configs", logo);
      break;
    case 1:
      display_setting = (display_setting + 1) % 2;
      prefs.putInt("display_configs", display_setting);
      if ( display_setting )
      {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
      }
      else
      {
        display.ssd1306_command(SSD1306_DISPLAYON);
      }
      break;
    case 2:
      SCREEN_HEIGHT = (SCREEN_HEIGHT == 64) ? 32 : 64;
      prefs.putInt("screen_height", SCREEN_HEIGHT);
      if ( SCREEN_HEIGHT == 32 )
      {
        display.ssd1306_command(SSD1306_SETCOMPINS);
        display.ssd1306_command(0x02);
        display.ssd1306_command(SSD1306_SETMULTIPLEX);
        display.ssd1306_command(0x1F);
      }
      else
      {
        display.ssd1306_command(SSD1306_SETCOMPINS);
        display.ssd1306_command(0x12);
        display.ssd1306_command(SSD1306_SETMULTIPLEX);
        display.ssd1306_command(0x3F);
      }
      break;
    case 3:
      buttons = (buttons + 1) % 3;
      prefs.putInt("buttons_configs", buttons);
      break;
    case 4:
      Separate_or_together = (Separate_or_together + 1) % 2;
      prefs.putInt("SorT_configs", Separate_or_together);
      break;
    case 5:
      nrf_pa = (nrf_pa + 1) % 4;
      prefs.putInt("PA_configs", nrf_pa);
      break;
    case 6:
      bluetooth_jam_method = (bluetooth_jam_method + 1) % 3;
      prefs.putInt("bt_configs", bluetooth_jam_method);
      break;
    case 7:
      drone_jam_method = (drone_jam_method + 1) % 2;
      prefs.putInt("drone_configs", drone_jam_method);
      break;
    case 8:
      misc_jam_method = (misc_jam_method + 1) % 2;
      prefs.putInt("misc_configs", misc_jam_method);
      break;
    case 9:
      access_point = (access_point + 1) % 2;
      prefs.putInt("AP_configs", access_point);
      break;
    case 10: {
      const uint32_t baudrate_list[] = {9600, 19200, 38400, 57600, 115200, 921600};
      const int      baudrate_count  = sizeof(baudrate_list) / sizeof(baudrate_list[0]);
      int            idx             = 0;
      for ( int i = 0; i < baudrate_count; i++ )
      {
        if ( baudrate_list[i] == (uint32_t)baudrate )
        {
          idx = i;
          break;
        }
      }
      idx      = (idx + delta + baudrate_count) % baudrate_count;
      baudrate = baudrate_list[idx];
      prefs.putInt("baudrate", baudrate);
      Serial.updateBaudRate(baudrate);
      break;
    }
    case 11:
      jam_delay += delta * 10;
      if ( jam_delay > 10000 )
        jam_delay = 0;
      if ( jam_delay < 0 )
        jam_delay = 10000;
      prefs.putInt("jam_delay", jam_delay);
      break;
    }
  };

  draw_settings();

  while ( true )
  {
    int total_items = (buttons == 0) ? 13 : 12;

    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();

    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() )
      {
        current_item = (current_item + 1) % total_items;
        draw_settings();
      }
      else if ( btnOK.isHolded() )
      {
        if ( current_item == 12 )
        {
          break;
        }
        else if ( current_item == 11 )
        {
        }
        else
        {
          apply_and_save_item(current_item, 1);
          draw_settings();
        }
      }
    }

    else if ( buttons == 1 )
    {
      if ( in_edit_mode )
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          apply_and_save_item(11, 1);
          draw_settings();
        }
        else if ( btnOK.isSingle() || btnOK.isHolded() )
        {
          in_edit_mode = false;
          draw_settings();
        }
      }
      else
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          current_item = (current_item + 1) % total_items;
          draw_settings();
        }
        else if ( btnOK.isSingle() )
        {
          if ( current_item == 11 )
          {
            in_edit_mode = true;
            draw_settings();
          }
          else
          {
            apply_and_save_item(current_item, 1);
            draw_settings();
          }
        }
        else if ( btnOK.isHolded() )
        {
          break;
        }
      }
    }

    else if ( buttons == 2 )
    {
      if ( in_edit_mode )
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          apply_and_save_item(11, 1);
          draw_settings();
        }
        else if ( btnPrevious.isSingle() || btnPrevious.isHold() )
        {
          if ( btnPrevious.isHold() )
            delay(100);
          apply_and_save_item(11, -1);
          draw_settings();
        }
        else if ( btnOK.isSingle() || btnOK.isHolded() )
        {
          in_edit_mode = false;
          draw_settings();
        }
      }
      else
      {
        if ( btnNext.isSingle() || btnNext.isHold() )
        {
          if ( btnNext.isHold() )
            delay(100);
          current_item = (current_item + 1) % total_items;
          draw_settings();
        }
        else if ( btnPrevious.isSingle() || btnPrevious.isHold() )
        {
          if ( btnPrevious.isHold() )
            delay(100);
          current_item = (current_item - 1 + total_items) % total_items;
          draw_settings();
        }
        else if ( btnOK.isSingle() )
        {
          if ( current_item == 11 )
          {
            in_edit_mode = true;
            draw_settings();
          }
          else
          {
            apply_and_save_item(current_item, 1);
            draw_settings();
          }
        }
        else if ( btnOK.isHolded() )
        {
          break;
        }
      }
    }
  }

  display.setTextColor(WHITE, BLACK);
  updateDisplay(menu_number);
}

void misc()
{
  int  flag         = 0;
  auto display_info = [&](String info) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Start");
    display.setCursor(96, 0);
    display.println("Stop");
    display.setCursor(52, 0);
    display.println("Width");
    display.setCursor(8, 10);
    display.println(String(channel1));
    display.setCursor(60, 10);
    display.println(String(channel2 - channel1));
    display.setCursor(100, 10);
    display.println(String(channel2));
    display.setCursor(0, 20);
    display.println(info);
    display.display();
  };

  display_info("");

  auto incrementChannel = [&](int &channel) {
    channel++;
    if ( channel > 125 )
    {
      channel = 0;
    }
  };
  auto reduceChannel = [&](int &channel) {
    channel--;
    if ( channel < 0 )
    {
      channel = 125;
    }
  };

  while ( true )
  {
    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();
    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() || btnOK.isHold() )
      {
        if ( btnOK.isHold() )
        {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      else if ( btnOK.isDouble() )
      {
        if ( flag == 0 )
        {
          flag = 1;
        }
        else
        {
          if ( channel1 > channel2 )
          {
            display_info("Error: Second < First");
            flag = 0;
          }
          else
          {
            display_info("Jamming Started");
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), nullptr, false, false, true, false, channel1, channel2);
            break;
          }
        }
      }
    }
    if ( buttons == 1 )
    {
      if ( btnNext.isSingle() || btnNext.isHold() )
      {
        if ( btnNext.isHold() )
        {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      else if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          flag = 1;
        }
        else
        {
          if ( channel1 > channel2 )
          {
            display_info("Error: Second < First");
            flag = 0;
          }
          else
          {
            display_info("Jamming Started");
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_misc_jammer_64 : bitmap_misc_jammer_32), false, false, true, false, channel1, channel2);
            break;
          }
        }
      }
    }
    if ( buttons == 2 )
    {
      if ( btnNext.isSingle() || btnNext.isHold() )
      {
        if ( btnNext.isHold() )
        {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      if ( btnPrevious.isSingle() || btnPrevious.isHold() )
      {
        if ( btnPrevious.isHold() )
        {
          delay(100);
        }
        reduceChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      else if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          flag = 1;
        }
        else
        {
          if ( channel1 > channel2 )
          {
            display_info("Error: Second < First");
            flag = 0;
          }
          else
          {
            display_info("Jamming Started");
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_misc_jammer_64 : bitmap_misc_jammer_32), false, false, true, false, channel1, channel2);
            break;
          }
        }
      }
    }
  }
}

void ble_select()
{
  auto display_info = [&](int flag) -> void {
    const uint8_t *bitmap =
        (flag == 0) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_advertising_channels_64 : bitmap_advertising_channels_32) : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_data_channels_64 : bitmap_data_channels_32);
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.display();
  };

  auto display_jam = [&](bool flag) -> void {
    display.clearDisplay();
    display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_ble_jam_64 : bitmap_ble_jam_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.display();
    if ( flag )
    {
      jamHandler(ble_advertising_jam, String("BLE AD Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_ble_jam_64 : bitmap_ble_jam_32), false, true);
    }
    else
    {
      jamHandler(ble_data_jam, String("BLE Data Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_ble_jam_64 : bitmap_ble_jam_32), true, false);
    }
  };

  int flag = 0;

  display_info(flag);

  while ( true )
  {
    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();

    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() )
      {
        flag = (flag + 1) % 2;
        display_info(flag);
      }
      if ( btnOK.isHolded() )
      {
        if ( flag == 0 )
        {
          display_jam(true);
          break;
        }
        if ( flag == 1 )
        {
          display_jam(false);
          break;
        }
      }
    }
    else if ( buttons == 1 )
    {
      if ( btnNext.isSingle() )
      {
        flag = (flag + 1) % 2;
        display_info(flag);
      }
      if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          display_jam(true);
          break;
        }
        if ( flag == 1 )
        {
          display_jam(false);
          break;
        }
      }
    }
    else if ( buttons == 2 )
    {
      if ( btnNext.isSingle() )
      {
        flag = (flag + 1) % 2;
        display_info(flag);
      }
      if ( btnPrevious.isSingle() )
      {
        flag = (flag - 1 + 2) % 2;
        display_info(flag);
      }
      if ( btnOK.isSingle() )
      {
        if ( flag == 0 )
        {
          display_jam(true);
          break;
        }
        if ( flag == 1 )
        {
          display_jam(false);
          break;
        }
      }
    }
  }
}

void wifi_select()
{
  auto display_info = [&](int flag, int wifi_points, int scroll_offset) -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("channel: " + String(flag));
    display.setCursor(0, 10);
    display.println("Wi-Fi APs: " + String(wifi_points));
    String apList   = APs_array[flag - 1];
    int    startIdx = 0;
    for ( int i = 0; i < scroll_offset; i++ )
    {
      int n = apList.indexOf('\n', startIdx);
      if ( n == -1 )
        break;
      startIdx = n + 1;
    }
    int maxLines;
    if ( SCREEN_HEIGHT == 32 )
    {
      maxLines = 2;
    }
    else
    {
      maxLines = 5;
    }

    int y = 20;
    for ( int i = 0; i < maxLines; i++ )
    {
      int    endIdx = apList.indexOf('\n', startIdx);
      String line;
      if ( endIdx == -1 )
      {
        line = apList.substring(startIdx);
      }
      else
      {
        line = apList.substring(startIdx, endIdx);
      }
      display.setCursor(0, y);
      display.println(line);
      y += 8;
      if ( endIdx == -1 )
        break;
      startIdx = endIdx + 1;
    }
    display.display();
  };

  auto scan_wifi = [&](int &channelCount, int *WiFi_channels) -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi APs");
    display.display();
    if ( access_point == 0 )
      WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    delay(100);
    int networks = scan_wifi_APs(WiFiScanChannels, false);
    display.setCursor(0, 10);
    display.println("Finded " + String(networks) + " APs");
    display.display();
    if ( access_point == 0 )
      WiFi.softAP(ssid.c_str(), password.c_str());
    delay(1000);
  };

  auto scan_wifi_channels = [&]() -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi ch's");
    display.display();
    if ( access_point == 0 )
      WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    delay(100);
    NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
    display.setCursor(0, 10);
    display.println("Finded " + String(NumberChannels) + " active ch's");
    display.display();
    if ( access_point == 0 )
      WiFi.softAP(ssid.c_str(), password.c_str());
    delay(1000);
  };

  auto count_lines = [](const String &s) -> int {
    int cnt = 1;
    for ( unsigned int i = 0; i < s.length(); i++ )
    {
      if ( s[i] == '\n' )
        cnt++;
    }
    return cnt;
  };

  int channelCount = 0;
  int menu_number  = 0;

  while ( true )
  {
    btnOK.tick();
    btnNext.tick();
    btnPrevious.tick();

    if ( buttons == 0 )
    {
      if ( btnOK.isSingle() )
      {
        menu_number = (menu_number + 1) % 4;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0)   ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_all_64 : bitmap_wifi_all_32)
            : (menu_number == 1) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_select_64 : bitmap_wifi_select_32)
            : (menu_number == 2) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_smart_attack_64 : bitmap_smart_attack_32)
                                 : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spam_64 : bitmap_beacon_spam_32);
        display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        display.display();
      }
      if ( btnOK.isHolded() )
      {
        if ( menu_number == 0 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_jam, String("WiFi Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32));
          }
          return;
        }
        else if ( menu_number == 1 )
        {
          scan_wifi(channelCount, WiFiScanChannels);

          int           flag          = 1;
          int           scroll_offset = 0;
          unsigned long last_scroll   = 0;
          int           total_lines   = count_lines(APs_array[flag - 1]);

          display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);

          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();

            if ( btnOK.isSingle() )
            {
              flag++;
              if ( flag > 14 )
                flag = 1;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( total_lines > ((SCREEN_HEIGHT == 32) ? 2 : 5) && millis() - last_scroll >= 1000 )
            {
              scroll_offset++;
              int maxLines = (SCREEN_HEIGHT == 32) ? 2 : 5;
              if ( scroll_offset + maxLines > total_lines )
              {
                scroll_offset = 0;
              }
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnOK.isHolded() )
            {
              btnNext.tick();
              btnOK.tick();
              btnPrevious.tick();
              while ( !btnOK.isSingle() )
              {
                display.clearDisplay();
                display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                Serial.println(flag - 1);
                bool exit = jamHandler(wifi_channel, "Jamming WiFi channel", decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true, false, false, flag - 1, 0);
                if ( btnOK.isSingle() || exit )
                  break;
                display.clearDisplay();
                display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                wifi_deauth_channel(flag);
              }
              return;
            }
          }
        }
        else if ( menu_number == 2 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          scan_wifi_channels();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_scan_jam, String("WiFi Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32));
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          display.clearDisplay();
          display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
          display.display();
          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();
            if ( btnOK.isSingle() )
            {
              menu_number = (menu_number + 1) % 2;
              display.clearDisplay();
              const uint8_t *bitmap =
                  (menu_number == 0) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32) : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_random_ssids_64 : bitmap_random_ssids_32);
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnOK.isHolded() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32));
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32));
                break;
              }
              break;
            }
          }
          return;
        }
      }
    }

    if ( buttons == 1 )
    {
      if ( btnNext.isSingle() )
      {
        menu_number = (menu_number + 1) % 4;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0)   ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_all_64 : bitmap_wifi_all_32)
            : (menu_number == 1) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_select_64 : bitmap_wifi_select_32)
            : (menu_number == 2) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_smart_attack_64 : bitmap_smart_attack_32)
                                 : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spam_64 : bitmap_beacon_spam_32);
        display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        display.display();
      }
      if ( btnOK.isSingle() )
      {
        if ( menu_number == 0 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_jam, String("WiFi Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32));
          }
          return;
        }
        else if ( menu_number == 1 )
        {
          scan_wifi(channelCount, WiFiScanChannels);

          int           flag          = 1;
          int           scroll_offset = 0;
          unsigned long last_scroll   = 0;
          int           total_lines   = count_lines(APs_array[flag - 1]);

          display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);

          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();

            if ( btnNext.isSingle() )
            {
              flag++;
              if ( flag > 14 )
                flag = 1;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( total_lines > ((SCREEN_HEIGHT == 32) ? 2 : 5) && millis() - last_scroll >= 1000 )
            {
              scroll_offset++;
              int maxLines = (SCREEN_HEIGHT == 32) ? 2 : 5;
              if ( scroll_offset + maxLines > total_lines )
              {
                scroll_offset = 0;
              }
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnOK.isSingle() )
            {
              btnNext.tick();
              btnOK.tick();
              btnPrevious.tick();
              while ( !btnOK.isSingle() )
              {
                display.clearDisplay();
                display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                Serial.println(flag - 1);
                bool exit = jamHandler(wifi_channel, "Jamming WiFi channel", decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true, false, false, flag - 1, 0);
                if ( btnOK.isSingle() || exit )
                  break;
                display.clearDisplay();
                display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                wifi_deauth_channel(flag);
              }
              return;
            }
          }
        }
        else if ( menu_number == 2 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          scan_wifi_channels();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_scan_jam, String("WiFi Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32));
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          display.clearDisplay();
          display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
          display.display();
          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();
            if ( btnNext.isSingle() )
            {
              menu_number = (menu_number + 1) % 2;
              display.clearDisplay();
              const uint8_t *bitmap =
                  (menu_number == 0) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32) : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_random_ssids_64 : bitmap_random_ssids_32);
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnOK.isSingle() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32));
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32));
                break;
              }
              break;
            }
          }
          return;
        }
      }
    }

    if ( buttons == 2 )
    {
      if ( btnNext.isSingle() )
      {
        menu_number = (menu_number + 1) % 4;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0)   ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_all_64 : bitmap_wifi_all_32)
            : (menu_number == 1) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_select_64 : bitmap_wifi_select_32)
            : (menu_number == 2) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_smart_attack_64 : bitmap_smart_attack_32)
                                 : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spam_64 : bitmap_beacon_spam_32);
        display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        display.display();
      }
      if ( btnPrevious.isSingle() )
      {
        menu_number = (menu_number - 1 + 4) % 4;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0)   ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_all_64 : bitmap_wifi_all_32)
            : (menu_number == 1) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_select_64 : bitmap_wifi_select_32)
            : (menu_number == 2) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_smart_attack_64 : bitmap_smart_attack_32)
                                 : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spam_64 : bitmap_beacon_spam_32);
        display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        display.display();
      }

      if ( btnOK.isSingle() )
      {
        if ( menu_number == 0 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_jam, String("WiFi Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32));
          }
          return;
        }
        else if ( menu_number == 1 )
        {
          scan_wifi(channelCount, WiFiScanChannels);

          int           flag          = 1;
          int           scroll_offset = 0;
          unsigned long last_scroll   = 0;
          int           total_lines   = count_lines(APs_array[flag - 1]);

          display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);

          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();

            if ( btnNext.isSingle() )
            {
              flag++;
              if ( flag > 14 )
                flag = 1;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnPrevious.isSingle() )
            {
              flag--;
              if ( flag < 1 )
                flag = 14;
              scroll_offset = 0;
              total_lines   = count_lines(APs_array[flag - 1]);
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( total_lines > ((SCREEN_HEIGHT == 32) ? 2 : 5) && millis() - last_scroll >= 1000 )
            {
              scroll_offset++;
              int maxLines = (SCREEN_HEIGHT == 32) ? 2 : 5;
              if ( scroll_offset + maxLines > total_lines )
              {
                scroll_offset = 0;
              }
              display_info(flag, WiFiScanChannels[flag - 1], scroll_offset);
              last_scroll = millis();
            }

            if ( btnOK.isSingle() )
            {
              btnNext.tick();
              btnOK.tick();
              btnPrevious.tick();
              while ( !btnOK.isSingle() )
              {
                display.clearDisplay();
                display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                bool exit = jamHandler(wifi_channel, "Jamming WiFi channel", decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true, false, false, flag - 1, 0);
                if ( btnOK.isSingle() || exit )
                  break;
                display.clearDisplay();
                display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                wifi_deauth_channel(flag);
              }
              return;
            }
          }
        }
        else if ( menu_number == 2 )
        {
          btnNext.tick();
          btnOK.tick();
          btnPrevious.tick();
          scan_wifi_channels();
          while ( !btnOK.isSingle() )
          {
            bool exit = jamHandler(wifi_scan_jam, String("WiFi Jam"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, true);
            if ( btnOK.isSingle() || exit )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32));
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          display.clearDisplay();
          display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
          display.display();
          while ( true )
          {
            btnOK.tick();
            btnNext.tick();
            btnPrevious.tick();
            if ( btnNext.isSingle() )
            {
              menu_number = (menu_number + 1) % 2;
              display.clearDisplay();
              const uint8_t *bitmap =
                  (menu_number == 0) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32) : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_random_ssids_64 : bitmap_random_ssids_32);
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnPrevious.isSingle() )
            {
              menu_number = (menu_number - 1 + 2) % 2;
              display.clearDisplay();
              const uint8_t *bitmap =
                  (menu_number == 0) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_list_ssids_64 : bitmap_list_ssids_32) : decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_random_ssids_64 : bitmap_random_ssids_32);
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnOK.isSingle() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32));
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32));
                break;
              }
              break;
            }
          }
          return;
        }
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  prefs.begin("configs", false);

  disableCore0WDT();

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  handlenRF24Init();
  if ( nrf24_count == 0 )
    handlenRF24Scan();

  bluetooth_jam_method = prefs.getInt("bt_configs", 0);
  drone_jam_method     = prefs.getInt("drone_configs", 1);
  display_setting      = prefs.getInt("display_configs", 0);
  wifi_jam_method      = prefs.getInt("wifi_configs", 0);
  nrf_pa               = prefs.getInt("PA_configs", 0);
  misc_jam_method      = prefs.getInt("misc_configs", 0);
  logo                 = prefs.getInt("logo_configs", 0);
  access_point         = prefs.getInt("AP_configs", 0);
  buttons              = prefs.getInt("buttons_configs", 0);
  Separate_or_together = prefs.getInt("SorT_configs", 0);
  jam_delay            = prefs.getInt("jam_delay", 0);
  SCREEN_HEIGHT        = prefs.getInt("screen_height", 64);
  baudrate             = prefs.getInt("baudrate", 115200);

  Serial.updateBaudRate(baudrate);

  if ( !prefs.isKey("SSIDs_Array") )
  {
    memset(SSIDs_Array, 0, sizeof(SSIDs_Array));
    for ( int i = 0; i < sizeof(default_SSIDs_Array) / sizeof(default_SSIDs_Array[0]) && i < 100; i++ )
    {
      if ( default_SSIDs_Array[i] == nullptr )
        break;
      strlcpy(SSIDs_Array[i], default_SSIDs_Array[i], 33);
    }
    prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));
  }
  else
  {
    prefs.getBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));

    if ( SSIDs_Array[0][0] == '\0' )
    {
      memset(SSIDs_Array, 0, sizeof(SSIDs_Array));
      for ( int i = 0; i < sizeof(default_SSIDs_Array) / sizeof(default_SSIDs_Array[0]) && i < 100; i++ )
      {
        if ( default_SSIDs_Array[i] == nullptr )
          break;
        strlcpy(SSIDs_Array[i], default_SSIDs_Array[i], 33);
      }
      prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));
    }
  }

  if ( access_point == 0 )
  {
    ssid     = prefs.getString("ssid", default_ssid);
    password = prefs.getString("password", default_password);

    WiFi.mode(WIFI_AP);
    delay(100);

    if ( WiFi.softAP(ssid.c_str(), password.c_str()) )
    {
      IPAddress apIp = WiFi.softAPIP();
      if ( apIp != IPAddress((uint32_t)0) )
      {
        dnsServerStarted = dnsServer.start(53, "*", apIp);
      }
    }

    server.on("/", handleRoot);
    server.on("/bluetooth_jam", []() { jamHandler(bluetooth_jam, String("Bluetooth Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_bluetooth_jam_64 : bitmap_bluetooth_jam_32), true, false); });
    server.on("/drone_jam", []() { jamHandler(drone_jam, String("Drone Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_drone_jam_64 : bitmap_drone_jam_32), true, false); });
    server.on("/wifi_jam", []() { jamHandler(wifi_jam, String("WiFi Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, false); });
    server.on("/wifi_deauth_all", []() { attackHandler(String("WiFi Deauthing"), wifi_deauth_all, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32)); });
    server.on("/wifi_scan_jam", []() { jamHandler(wifi_scan_jam, String("WiFi Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_jam_64 : bitmap_wifi_jam_32), false, false, false, true); });
    server.on("/wifi_deauth_scan", []() { attackScanHandler(String("WiFi Deauthing"), wifi_deauth_scan, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_deauth_64 : bitmap_wifi_deauth_32)); });
    server.on("/wifi_random_spam", []() { attackScanHandler(String("WiFi Beacon Spam"), wifi_beacon_spam_random, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32)); });
    server.on("/wifi_array_spam", []() { attackScanHandler(String("WiFi Beacon Spam"), wifi_beacon_spam_array, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_beacon_spaming_64 : bitmap_beacon_spaming_32)); });
    server.on("/ble_advertising_jam", []() { jamHandler(ble_advertising_jam, String("BLE Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_ble_jam_64 : bitmap_ble_jam_32), false, false); });
    server.on("/ble_data_jam", []() { jamHandler(ble_data_jam, String("BLE Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_ble_jam_64 : bitmap_ble_jam_32), true, false); });
    server.on("/zigbee_jam", []() { jamHandler(zigbee_jam, String("Zigbee Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_zigbee_jam_64 : bitmap_zigbee_jam_32), false, false); });
    server.on("/web_serial", []() { sendHtmlAndExecute(html_webserial, html_webserial_len); });
    server.on("/misc_jam", miscChannelsHandler);
    server.on("/wifi_selected_jam", wifiChannelsHandler);
    server.on("/wifi_selected_deauth", wifiDeauthChannelsHandler);
    server.on("/rescan", RescanHandler);

    server.on("/api/status", handleApiStatus);
    server.on("/api/settings", handleApiSettings);
    server.on("/api/version", handleApiVersion);

    server.on("/settings", []() {
      server.sendHeader("Content-Encoding", "gzip");
      server.send_P(200, "text/html", (const char *)html_settings, html_settings_len);
    });
    server.on("/OTA", []() {
      server.sendHeader("Content-Encoding", "gzip");
      server.send_P(200, "text/html", (const char *)html_ota, html_ota_len);
    });

    server.on("/WebCommand", HandleWebCommand);

    server.on("/generate_204", handleRoot);
    server.on("/redirect", handleRoot);
    server.on("/hotspot-detect.html", handleRoot);
    server.on("/canonical.html", handleRoot);
    server.on("/ncsi.txt", handleRoot);

    server.on(
        "/update", HTTP_POST, []() {
          if ( Update.end(true) )
          {
            Serial.printf("Update Success: %u bytes\n", Update.size());
            server.send(200, "text/plain", "Update Success");
            delay(100);
            display.clearDisplay();
            display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_pls_reboot_64 : bitmap_pls_reboot_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
            display.display();
            delay(2000);
            ESP.restart();
          }
          else
          {
            Update.printError(Serial);
            server.send(500, "text/plain", "Update Failed");
          }
        },
        handleFileUpload
    );

    server.on("/set_settings", []() { handlerSetSettings(); });

    server.begin();
    webServerStarted = true;
  }
  Serial.println(logotype + "\n\n");

  btnOK.setTickMode(false);
  btnNext.setTickMode(false);
  btnPrevious.setTickMode(false);
  btnOK.setClickTimeout(200);
  btnNext.setClickTimeout(200);
  btnPrevious.setClickTimeout(200);
  btnOK.setTimeout(600);
  btnNext.setTimeout(600);
  btnPrevious.setTimeout(600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if ( SCREEN_HEIGHT == 32 )
  {
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x02);
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x1F);
  }
  else
  {
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x12);
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x3F);
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  if ( display_setting )
  {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
  display.clearDisplay();
  if ( logo == 0 )
  {
    display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_logo_64 : bitmap_logo_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  }
  else
  {
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi APs");
  }
  display.display();
  Serial.println("  Scanning Wi-Fi APs");

  int networks = scan_wifi_APs(WiFiScanChannels, false);

  if ( logo != 0 )
  {
    display.setCursor(0, 10);
    display.println("Finded " + String(networks) + " APs");
    display.display();
    delay(1000);
  }
  Serial.println("  Finded " + String(networks) + " APs");

  Serial.println("  help ==> Displays a list of available commands\n");

  display.setCursor(0, 0);
  display.clearDisplay();
  display.drawBitmap(0, 0, decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_bluetooth_jammer_64 : bitmap_bluetooth_jammer_32), SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
}

void executeAction(int menuNum)
{
  if ( nrf24_count <= 0 )
  {
    display.clearDisplay();
    display.setCursor(40, 0);
    display.println("WARNING");

    display.setCursor(5, 10);
    display.println("nRF24 not configured");

    display.setCursor(7, 20);
    display.println("Configure in web UI");

    display.display();
    while ( nrf24_count <= 0 )
    {
      if ( webServerStarted )
      {
        server.handleClient();
      }
      if ( dnsServerStarted )
      {
        dnsServer.processNextRequest();
      }
      delay(100);
    }
  }
  if ( menuNum == 5 )
  {
    misc();
    updateDisplay(menu_number);
    return;
  }
  if ( menuNum == 3 )
  {
    ble_select();
    updateDisplay(menu_number);
    return;
  }

  display.clearDisplay();
  const uint8_t *bitmap = (menu_number == 0)   ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_bluetooth_jam_64 : bitmap_bluetooth_jam_32)
                          : (menu_number == 1) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_drone_jam_64 : bitmap_drone_jam_32)
                          : (menu_number == 2) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_wifi_all_64 : bitmap_wifi_all_32)
                          : (menu_number == 4) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_zigbee_jam_64 : bitmap_zigbee_jam_32)
                          : (menu_number == 6) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_settings_64 : bitmap_settings_32)
                          : (menu_number == 7) ? decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_pls_reboot_64 : bitmap_pls_reboot_32)
                                               : NULL;

  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();

  switch ( menu_number )
  {
  case 0:
    jamHandler(bluetooth_jam, String("Bluetooth Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_bluetooth_jam_64 : bitmap_bluetooth_jam_32), true, false);
    break;
  case 1:
    jamHandler(drone_jam, String("Drone Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_drone_jam_64 : bitmap_drone_jam_32), true, false);
    break;
  case 2:
    wifi_select();
    break;
  case 4:
    jamHandler(zigbee_jam, String("Zigbee Jamming"), decodeLZSS((SCREEN_HEIGHT == 64) ? bitmap_zigbee_jam_64 : bitmap_zigbee_jam_32), false, false);
    break;
  case 6:
    settings();
    break;
  case 7:
    prefs.putInt("AP_configs", 0);
    break;
  default:
    break;
  }
  if ( menuNum != 6 )
    updateDisplay(menuNum);
}

void loop()
{
  btnOK.tick();
  btnNext.tick();
  btnPrevious.tick();

  SerialCommands();

  if ( access_point == 0 )
  {
    if ( webServerStarted )
    {
      server.handleClient();
    }
    if ( dnsServerStarted )
    {
      dnsServer.processNextRequest();
    }
  }
  if ( buttons == 0 )
  {
    if ( btnOK.isSingle() )
    {
      menu_number = (menu_number + 1) % 7;
      updateDisplay(menu_number);
    }
    if ( btnOK.isHolded() )
    {
      executeAction(menu_number);
      btnOK.resetStates();
    }
  }
  else if ( buttons == 1 )
  {
    if ( btnOK.isSingle() )
    {
      executeAction(menu_number);
    }
    if ( btnNext.isSingle() )
    {
      menu_number = (menu_number + 1) % 7;
      updateDisplay(menu_number);
    }
  }
  else if ( buttons == 2 )
  {
    if ( btnOK.isSingle() )
    {
      executeAction(menu_number);
    }

    if ( btnNext.isSingle() )
    {
      menu_number = (menu_number + 1) % 7;
      updateDisplay(menu_number);
    }

    if ( btnPrevious.isSingle() )
    {
      menu_number = (menu_number - 1 + 7) % 7;
      updateDisplay(menu_number);
    }
  }
}