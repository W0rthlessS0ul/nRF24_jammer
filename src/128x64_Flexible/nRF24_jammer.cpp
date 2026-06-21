#include "bitmap.h"
#include "config.h"
#include "html.h"
#include "jam.h"
#include "scan.h"
#include "serial.h"

void handleRoot()
{
  String main_html = FPSTR(html);
  switch ( bluetooth_jam_method )
  {
  case 0:
    main_html.replace("[||]EdItAbLe TeXt[||]", "21 channels");
    break;
  case 1:
    main_html.replace("[||]EdItAbLe TeXt[||]", "80 channels");
    break;
  case 2:
    main_html.replace("[||]EdItAbLe TeXt[||]", "80 channels");
    break;
  }
  if ( nrf24_count <= 0 )
  {
    main_html.replace("if (false) { //IsModulesConfigured", "if (true) { //IsModulesConfigured");
  }

  server.send(200, "text/html", main_html.c_str());

  if ( nrf24_count <= 0 )
  {
    main_html.replace("if (true) { //IsModulesConfigured", "if (false) { //IsModulesConfigured");
  }
  switch ( bluetooth_jam_method )
  {
  case 0:
    main_html.replace("21 channels", "[||]EdItAbLe TeXt[||]");
    break;
  case 1:
    main_html.replace("80 channels", "[||]EdItAbLe TeXt[||]");
    break;
  case 2:
    main_html.replace("80 channels", "[||]EdItAbLe TeXt[||]");
    break;
  }
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
  const uint8_t *bitmap = (menu_number == 0)   ? bitmap_bluetooth_jammer
                          : (menu_number == 1) ? bitmap_drone_jammer
                          : (menu_number == 2) ? bitmap_wifi_attacks
                          : (menu_number == 3) ? bitmap_ble_jammer
                          : (menu_number == 4) ? bitmap_zigbee_jammer
                          : (menu_number == 5) ? bitmap_misc_jammer
                                               : bitmap_access_point;
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
}

void handlernRF24Pins()
{
  nrf24_count     = server.arg("count").toInt();
  String ceStr    = server.arg("ce");
  String csnStr   = server.arg("csn");
  auto   SplitStr = [](const String &str, int *arr, int maxCount) {
    int idx   = 0;
    int start = 0;
    while ( idx < maxCount )
    {
      int sep = str.indexOf('|', start);
      if ( sep == -1 )
        sep = str.length();
      String part = str.substring(start, sep);
      arr[idx++]  = part.toInt();
      if ( sep == str.length() )
        break;
      start = sep + 1;
    }
  };
  SplitStr(ceStr, ce_pins, nrf24_count);
  SplitStr(csnStr, csn_pins, nrf24_count);

  prefs.putInt("nRF24_count", nrf24_count);
  prefs.putBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.putBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handlerSSIDsArray()
{
  String ssidsStr = server.arg("ssids");

  memset(SSIDs_Array, 0, sizeof(SSIDs_Array));

  int idx   = 0;
  int start = 0;
  while ( idx < 100 && start < ssidsStr.length() )
  {
    int end = ssidsStr.indexOf('|', start);
    if ( end == -1 )
      end = ssidsStr.length();

    String ssid = ssidsStr.substring(start, end);
    ssid.trim();

    if ( ssid.length() > 31 )
      ssid = ssid.substring(0, 31);

    if ( ssid.length() > 0 )
    {
      strlcpy(SSIDs_Array[idx], ssid.c_str(), 33);
      idx++;
    }

    start = end + 1;
    if ( end == ssidsStr.length() )
      break;
  }

  prefs.putBytes("SSIDs_Array", SSIDs_Array, sizeof(SSIDs_Array));

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handlenRF24Init()
{
  nrf24_count = prefs.getInt("nRF24_count", 0);

  prefs.getBytes("nRF24_ce_pins", ce_pins, sizeof(ce_pins));
  prefs.getBytes("nRF24_csn_pins", csn_pins, sizeof(csn_pins));
}

void WiFiChannelHandler()
{
  String html      = FPSTR(html_wifi_channel);
  String ssidsJson = "[";
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String escaped = APs_array[ch - 1];
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.replace("\n", "\\n");
    ssidsJson += "\"" + escaped + "\"";
    if ( ch < 14 )
      ssidsJson += ",";
  }
  ssidsJson += "]";

  for ( int ch = 1; ch <= 14; ch++ )
  {
    String search  = "let ch" + String(ch) + " = 0;";
    String replace = "let ch" + String(ch) + " = " + String(WiFiScanChannels[ch - 1]) + ";";
    html.replace(search, replace);
  }

  String ssidPlaceholder = "let ssidsByChannel = [];";
  String ssidReplacement = "let ssidsByChannel = " + ssidsJson + ";";
  html.replace(ssidPlaceholder, ssidReplacement);

  server.send(200, "text/html", html.c_str());

  html.replace(ssidReplacement, ssidPlaceholder);
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String search  = "let ch" + String(ch) + " = " + String(WiFiScanChannels[ch - 1]) + ";";
    String replace = "let ch" + String(ch) + " = 0;";
    html.replace(search, replace);
  }
}

void WiFiDeauthChannelHandler()
{
  String html      = FPSTR(html_wifi_channel);
  String ssidsJson = "[";
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String escaped = APs_array[ch - 1];
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.replace("\n", "\\n");
    ssidsJson += "\"" + escaped + "\"";
    if ( ch < 14 )
      ssidsJson += ",";
  }
  ssidsJson += "]";

  for ( int ch = 1; ch <= 14; ch++ )
  {
    String search  = "let ch" + String(ch) + " = 0;";
    String replace = "let ch" + String(ch) + " = " + String(WiFiScanChannels[ch - 1]) + ";";
    html.replace(search, replace);
  }

  String ssidPlaceholder = "let ssidsByChannel = [];";
  String ssidReplacement = "let ssidsByChannel = " + ssidsJson + ";";
  html.replace(ssidPlaceholder, ssidReplacement);
  html.replace("/wifi_selected_jam", "/wifi_selected_deauth");

  server.send(200, "text/html", html.c_str());

  html.replace("/wifi_selected_deauth", "/wifi_selected_jam");
  html.replace(ssidReplacement, ssidPlaceholder);
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String search  = "let ch" + String(ch) + " = " + String(WiFiScanChannels[ch - 1]) + ";";
    String replace = "let ch" + String(ch) + " = 0;";
    html.replace(search, replace);
  }
}

void RescanHandler()
{
  scan_wifi_APs(WiFiScanChannels, false);
  handleRoot();
}

void sendHtmlAndExecute(const char *htmlResponse, void (*action)() = nullptr)
{
  server.send(200, "text/html", htmlResponse);
  delay(500);
  if ( action )
    action();
}

void attackHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap)
{
  String html = FPSTR(html_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  html.replace("[||]EdItAbLe TeXt[||]", htmlResponse);
  sendHtmlAndExecute(html.c_str(), attackFunction);
  html.replace(htmlResponse, "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void attackScanHandler(String htmlResponse, void (*attackFunction)(), const unsigned char *bitmap)
{
  String html = FPSTR(html_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  html.replace("[||]EdItAbLe TeXt[||]", htmlResponse);
  server.send(200, "text/html", html.c_str());
  html.replace(htmlResponse, "[||]EdItAbLe TeXt[||]");
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
  int    channel1 = server.arg("start").toInt();
  int    channel2 = server.arg("stop").toInt();
  String html     = FPSTR(html_jam);
  html.replace("[||]EdItAbLe TeXt[||]", "Jamming from " + String(channel1) + " to " + String(channel2));
  sendHtmlAndExecute(html.c_str());
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
  jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), bitmap_misc_jammer, false, false, true, false, channel1, channel2);
  html.replace("Jamming from " + String(channel1) + " to " + String(channel2), "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void wifiChannelsHandler()
{
  int channel = server.arg("channel").toInt();

  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_wifi_jam, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  String html = FPSTR(html_jam);
  html.replace("[||]EdItAbLe TeXt[||]", "Jamming " + String(channel) + " WiFi Channel");
  sendHtmlAndExecute(html.c_str());
  jamHandler(wifi_channel, "Jamming WiFi channel", bitmap_wifi_jam, false, false, false, false, channel - 1, 0);
  html.replace("Jamming " + String(channel) + " WiFi Channel", "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void wifiDeauthChannelsHandler()
{
  int channel = server.arg("channel").toInt();

  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_wifi_deauth, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  String html = FPSTR(html_jam);
  html.replace("[||]EdItAbLe TeXt[||]", "Attacking " + String(channel) + " WiFi Channel");
  sendHtmlAndExecute(html.c_str());
  wifi_deauth_channel(channel);
  html.replace("Attacking " + String(channel) + " WiFi Channel", "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void nRF24SettingsHandler()
{
  String html          = FPSTR(html_nrf24_settings);
  String modulesScript = "<script>window.currentModules = [";
  for ( int i = 0; i < nrf24_count; i++ )
  {
    modulesScript += "{ce: " + String(ce_pins[i]) + ", csn: " + String(csn_pins[i]) + "}";
    if ( i < nrf24_count - 1 )
    {
      modulesScript += ",";
    }
  }
  modulesScript += "];</script>";
  html.replace("</body>", modulesScript + "</body>");
  sendHtmlAndExecute(html.c_str());
}

void settingsHandler(String htmlResponse, int index, bool editable, int SettingNumber)
{
  switch ( SettingNumber )
  {
  case 0:
    break;
  case 1:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_buttons_settings);
    break;
  case 2:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_logo_settings);
    break;
  case 3:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_misc_settings);
    break;
  case 4:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_display_settings);
    break;
  case 5:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_bluetooth_settings);
    break;
  case 6:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_drone_settings);
    break;
  case 7:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_separate_or_together);
    break;
  case 8:
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_nrf_pa_settings);
    break;
  case 9: {
    String temp;
    if ( prefs.isKey("SSIDs_Array") )
    {
      for ( int i = 0; i < 100; i++ )
      {
        temp += String(SSIDs_Array[i]);
        if ( i == 99 || SSIDs_Array[i + 1][0] == '\0' )
          break;
        temp += '\n';
      }
    }
    else
      temp = "";
    htmlResponse.replace("[||]EdItAbLe TeXt[||]", temp);
    break;
  }
  }
  if ( index == 1010110 )
  {
    htmlResponse.replace("<div class=\"version-badge\">version</div>", "<div class=\"version-badge\">" + String(Version_Number) + "</div>");
  }
  else if ( editable )
    htmlResponse.replace("<!-- " + String(index) + " --><button class=\"btn\"", "<!-- " + String(index) + " --><button class=\"btn btn_installed\"");

  sendHtmlAndExecute(htmlResponse.c_str());

  switch ( SettingNumber )
  {
  case 0:
    break;
  case 1:
    htmlResponse.replace(html_buttons_settings, "[||]EdItAbLe TeXt[||]");
    break;
  case 2:
    htmlResponse.replace(html_logo_settings, "[||]EdItAbLe TeXt[||]");
    break;
  case 3:
    htmlResponse.replace(html_misc_settings, "[||]EdItAbLe TeXt[||]");
    break;
  case 4:
    htmlResponse.replace(html_display_settings, "[||]EdItAbLe TeXt[||]");
    break;
  case 5:
    htmlResponse.replace(html_bluetooth_settings, "[||]EdItAbLe TeXt[||]");
    break;
  case 6:
    htmlResponse.replace(html_drone_settings, "[||]EdItAbLe TeXt[||]");
    break;
  case 7:
    htmlResponse.replace(html_separate_or_together, "[||]EdItAbLe TeXt[||]");
    break;
  case 8:
    htmlResponse.replace(html_nrf_pa_settings, "[||]EdItAbLe TeXt[||]");
    break;
  }
  if ( index == 1010110 )
  {
    htmlResponse.replace("<div class=\"version-badge\">" + String(Version_Number) + "</div>", "<div class=\"version-badge\">version</div>");
  }
  else if ( editable )
    htmlResponse.replace("<!-- " + String(index) + " --><button class=\"btn btn_installed\"", "<!-- " + String(index) + " --><button class=\"btn\"");
}

void storeEEPROMAndReset(const char *index, int value, int &targetVar)
{
  settingsHandler(String(FPSTR(html_pls_reboot)), 0, false, 0);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();

  prefs.putInt(index, value);
  targetVar = value;
  ESP.restart();
}

void storeEEPROMAndSet(const char *index, int value, int &targetVar)
{
  prefs.putInt(index, value);
  targetVar = value;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void registerRoute(const char *path, void (*handler)())
{
  server.on(path, handler);
}

void saveWiFiSettings(const char *new_ssid, const char *new_password)
{
  prefs.putString("ssid", new_ssid);

  prefs.putString("password", new_password);
}

void handleSaveWiFiSettings()
{
  String new_ssid     = server.arg("ssid");
  String new_password = server.arg("password");

  if ( new_ssid == "" && new_password == "" )
  {
    server.send_P(200, "text/html", html_pls_reboot);
    storeEEPROMAndSet("AP_configs", 1, access_point);
    return;
  }

  saveWiFiSettings(new_ssid.c_str(), new_password.c_str());

  server.send_P(200, "text/html", html_pls_reboot);
  delay(1000);
  ESP.restart();
}

void handleResetWiFiSettings()
{
  saveWiFiSettings(default_ssid, default_password);

  server.send_P(200, "text/html", html_pls_reboot);
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
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), bitmap_misc_jammer, false, false, true, false, channel1, channel2);
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
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), bitmap_misc_jammer, false, false, true, false, channel1, channel2);
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
            jamHandler(misc_jam, "Jamming from " + String(channel1) + " to " + String(channel2), bitmap_misc_jammer, false, false, true, false, channel1, channel2);
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
        (flag == 0) ? bitmap_advertising_channels : bitmap_data_channels;
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.display();
  };

  auto display_jam = [&](bool flag) -> void {
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap_ble_jam, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.display();
    if ( flag )
    {
      ble_advertising_jam();
    }
    else
    {
      ble_data_jam();
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
    String current_ssid     = prefs.getString("ssid", default_ssid);
    String current_password = prefs.getString("password", default_password);
    if ( access_point == 0 )
      WiFi.softAP(current_ssid.c_str(), current_password.c_str());
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
    String current_ssid     = prefs.getString("ssid", default_ssid);
    String current_password = prefs.getString("password", default_password);
    if ( access_point == 0 )
      WiFi.softAP(current_ssid.c_str(), current_password.c_str());
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
            (menu_number == 0)   ? bitmap_wifi_all
            : (menu_number == 1) ? bitmap_wifi_select
            : (menu_number == 2) ? bitmap_smart_attack
                                 : bitmap_beacon_spam;
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
            jamHandler(wifi_jam, String("WiFi Jam"), bitmap_wifi_jam, false, true);
            if ( btnOK.isSingle() )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, bitmap_wifi_deauth);
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
                display.drawBitmap(0, 0, bitmap_wifi_jam, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                Serial.println(flag - 1);
                jamHandler(wifi_channel, "Jamming WiFi channel", bitmap_wifi_jam, false, false, false, false, flag - 1, 0);
                if ( btnOK.isSingle() )
                  break;
                display.clearDisplay();
                display.drawBitmap(0, 0, bitmap_wifi_deauth, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
            jamHandler(wifi_scan_jam, String("WiFi Jam"), bitmap_wifi_jam, false, true);
            if ( btnOK.isSingle() )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, bitmap_wifi_deauth);
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_list_ssids, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
                  (menu_number == 0) ? bitmap_list_ssids : bitmap_random_ssids;
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnOK.isHolded() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, bitmap_beacon_spaming);
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, bitmap_beacon_spaming);
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
            (menu_number == 0)   ? bitmap_wifi_all
            : (menu_number == 1) ? bitmap_wifi_select
            : (menu_number == 2) ? bitmap_smart_attack
                                 : bitmap_beacon_spam;
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
            jamHandler(wifi_jam, String("WiFi Jam"), bitmap_wifi_jam, false, true);
            if ( btnOK.isSingle() )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, bitmap_wifi_deauth);
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
                display.drawBitmap(0, 0, bitmap_wifi_jam, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                Serial.println(flag - 1);
                jamHandler(wifi_channel, "Jamming WiFi channel", bitmap_wifi_jam, false, false, false, false, flag - 1, 0);
                if ( btnOK.isSingle() )
                  break;
                display.clearDisplay();
                display.drawBitmap(0, 0, bitmap_wifi_deauth, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
            jamHandler(wifi_scan_jam, String("WiFi Jam"), bitmap_wifi_jam, false, true);
            if ( btnOK.isSingle() )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, bitmap_wifi_deauth);
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_list_ssids, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
                  (menu_number == 0) ? bitmap_list_ssids : bitmap_random_ssids;
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnOK.isSingle() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, bitmap_beacon_spaming);
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, bitmap_beacon_spaming);
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
            (menu_number == 0)   ? bitmap_wifi_all
            : (menu_number == 1) ? bitmap_wifi_select
            : (menu_number == 2) ? bitmap_smart_attack
                                 : bitmap_beacon_spam;
        display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        display.display();
      }
      if ( btnPrevious.isSingle() )
      {
        menu_number = (menu_number - 1 + 4) % 4;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0)   ? bitmap_wifi_all
            : (menu_number == 1) ? bitmap_wifi_select
            : (menu_number == 2) ? bitmap_smart_attack
                                 : bitmap_beacon_spam;
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
            jamHandler(wifi_jam, String("WiFi Jam"), bitmap_wifi_jam, false, true);
            if ( btnOK.isSingle() )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_all, bitmap_wifi_deauth);
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
                display.drawBitmap(0, 0, bitmap_wifi_jam, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
                display.display();
                jamHandler(wifi_channel, "Jamming WiFi channel", bitmap_wifi_jam, false, false, false, false, flag - 1, 0);
                if ( btnOK.isSingle() )
                  break;
                display.clearDisplay();
                display.drawBitmap(0, 0, bitmap_wifi_deauth, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
            jamHandler(wifi_scan_jam, String("WiFi Jam"), bitmap_wifi_jam, false, true);
            if ( btnOK.isSingle() )
              break;
            attackHandler(String("WiFi Deauthing"), wifi_deauth_scan, bitmap_wifi_deauth);
          }
          return;
        }
        else if ( menu_number == 3 )
        {
          menu_number = 0;
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_list_ssids, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
                  (menu_number == 0) ? bitmap_list_ssids : bitmap_random_ssids;
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnPrevious.isSingle() )
            {
              menu_number = (menu_number - 1 + 2) % 2;
              display.clearDisplay();
              const uint8_t *bitmap =
                  (menu_number == 0) ? bitmap_list_ssids : bitmap_random_ssids;
              display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
              display.display();
            }
            if ( btnOK.isSingle() )
            {
              switch ( menu_number )
              {
              case 0:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_array, bitmap_beacon_spaming);
                break;
              case 1:
                attackHandler(String("WiFi Beacon Spaming"), wifi_beacon_spam_random, bitmap_beacon_spaming);
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

void access_poin_off()
{
  settingsHandler(String(FPSTR(html_pls_reboot)), 0, false, 0);
  storeEEPROMAndSet("AP_configs", 1, access_point);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  delay(1000);
  ESP.restart();
}

void setup()
{
  Serial.begin(115200);
  prefs.begin("configs", false);

  disableCore0WDT();

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  handlenRF24Init();

  bluetooth_jam_method = prefs.getInt("bl_jam_configs", 0);
  drone_jam_method     = prefs.getInt("drone_jam_configs", 0);
  display_setting      = prefs.getInt("display_configs", 0);
  wifi_jam_method      = prefs.getInt("wifi_jam_configs", 0);
  nrf_pa               = prefs.getInt("PA_configs", 0);
  misc_jam_method      = prefs.getInt("mics_jam_configs", 0);
  logo                 = prefs.getInt("logo_configs", 0);
  access_point         = prefs.getInt("AP_configs", 0);
  buttons              = prefs.getInt("buttons_configs", 0);
  Separate_or_together = prefs.getInt("SorT_configs", 0);

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
    String current_ssid     = prefs.getString("ssid", default_ssid);
    String current_password = prefs.getString("password", default_password);

    WiFi.mode(WIFI_AP);
    delay(100);

    if ( WiFi.softAP(current_ssid.c_str(), current_password.c_str()) )
    {
      IPAddress apIp = WiFi.softAPIP();
      if ( apIp != IPAddress((uint32_t)0) )
      {
        dnsServerStarted = dnsServer.start(53, "*", apIp);
      }
    }

    registerRoute("/", handleRoot);
    registerRoute("/bluetooth_jam", []() { jamHandler(bluetooth_jam, String("Bluetooth Jamming"), bitmap_bluetooth_jam, true, false); });
    registerRoute("/drone_jam", []() { jamHandler(drone_jam, String("Drone Jamming"), bitmap_drone_jam, true, false); });
    registerRoute("/wifi_jam", []() { jamHandler(wifi_jam, String("WiFi Jamming"), bitmap_wifi_jam, false, false); });
    registerRoute("/wifi_deauth_all", []() { attackHandler(String("WiFi Deauthing"), wifi_deauth_all, bitmap_wifi_deauth); });
    registerRoute("/wifi_scan_jam", []() { jamHandler(wifi_scan_jam, String("WiFi Jamming"), bitmap_wifi_jam, false, false, false, true); });
    registerRoute("/wifi_deauth_scan", []() { attackScanHandler(String("WiFi Deauthing"), wifi_deauth_scan, bitmap_wifi_deauth); });
    registerRoute("/wifi_random_spam", []() { attackScanHandler(String("WiFi Beacon Spam"), wifi_beacon_spam_random, bitmap_beacon_spaming); });
    registerRoute("/wifi_array_spam", []() { attackScanHandler(String("WiFi Beacon Spam"), wifi_beacon_spam_array, bitmap_beacon_spaming); });
    registerRoute("/ble_advertising_jam", []() { jamHandler(ble_advertising_jam, String("BLE Jamming"), bitmap_ble_jam, false, false); });
    registerRoute("/ble_data_jam", []() { jamHandler(ble_data_jam, String("BLE Jamming"), bitmap_ble_jam, true, false); });
    registerRoute("/zigbee_jam", []() { jamHandler(zigbee_jam, String("Zigbee Jamming"), bitmap_zigbee_jam, false, false); });
    registerRoute("/misc_jammer", []() { sendHtmlAndExecute(html_misc_jammer); });
    registerRoute("/web_serial", []() { sendHtmlAndExecute(html_webserial); });
    registerRoute("/misc_jam", miscChannelsHandler);
    registerRoute("/wifi_selected_jam", wifiChannelsHandler);
    registerRoute("/wifi_selected_deauth", wifiDeauthChannelsHandler);
    registerRoute("/rescan", RescanHandler);

    registerRoute("/setting_display", []() { settingsHandler(html_settings, display_setting, true, 4); });
    registerRoute("/setting_bluetooth_jam", []() { settingsHandler(html_settings, bluetooth_jam_method, true, 5); });
    registerRoute("/setting_drone_jam", []() { settingsHandler(html_settings, drone_jam_method, true, 6); });
    registerRoute("/setting_separate_together", []() { settingsHandler(html_settings, Separate_or_together, true, 7); });
    registerRoute("/setting_misc_jam", []() { settingsHandler(html_settings, misc_jam_method, true, 3); });
    registerRoute("/setting_logo", []() { settingsHandler(html_settings, logo, true, 2); });
    registerRoute("/setting_buttons", []() { settingsHandler(html_settings, buttons, true, 1); });
    registerRoute("/setting_nrf_pa", []() { settingsHandler(html_settings, nrf_pa, true, 8); });
    registerRoute("/setting_array_spam", []() { settingsHandler(html_spam_array, 0, false, 9); });

    registerRoute("/OTA", []() { settingsHandler(html_ota, 1010110, false, 0); });
    registerRoute("/ble_select", []() { settingsHandler(html_ble_select, 0, false, 0); });
    registerRoute("/wifi_select", []() { settingsHandler(html_wifi_select, 0, false, 0); });
    registerRoute("/wifi_channel", WiFiChannelHandler);
    registerRoute("/wifi_deauth_channel", WiFiDeauthChannelHandler);
    registerRoute("/wifi_settings", []() { settingsHandler(html_wifi_settings, wifi_jam_method, true, 0); });
    registerRoute("/nrf24_settings", []() { nRF24SettingsHandler(); });

    registerRoute("/WebCommand", HandleWebCommand);

    registerRoute("/generate_204", handleRoot);
    registerRoute("/redirect", handleRoot);
    registerRoute("/hotspot-detect.html", handleRoot);
    registerRoute("/canonical.html", handleRoot);
    registerRoute("/ncsi.txt", handleRoot);

    server.on(
        "/update", HTTP_POST, []() {
          if ( Update.end(true) )
          {
            Serial.printf("Update Success: %u bytes\n", Update.size());
            server.send(200, "text/plain", "Update Success");
            delay(100);
            display.clearDisplay();
            display.drawBitmap(0, 0, bitmap_pls_reboot, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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

    server.on("/save_wifi_settings", handleSaveWiFiSettings);
    server.on("/reset_wifi_settings", handleResetWiFiSettings);

    registerRoute("/bluetooth_method_0", []() { storeEEPROMAndSet("bl_jam_configs", 0, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_1", []() { storeEEPROMAndSet("bl_jam_configs", 1, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_2", []() { storeEEPROMAndSet("bl_jam_configs", 2, bluetooth_jam_method); });
    registerRoute("/drone_method_0", []() { storeEEPROMAndSet("drone_jam_configs", 0, drone_jam_method); });
    registerRoute("/drone_method_1", []() { storeEEPROMAndSet("drone_jam_configs", 1, drone_jam_method); });
    registerRoute("/separate_or_together_method_0", []() { storeEEPROMAndSet("SorT_configs", 0, Separate_or_together); });
    registerRoute("/separate_or_together_method_1", []() { storeEEPROMAndSet("SorT_configs", 1, Separate_or_together); });
    registerRoute("/misc_method_0", []() { storeEEPROMAndSet("mics_jam_configs", 0, misc_jam_method); });
    registerRoute("/misc_method_1", []() { storeEEPROMAndSet("mics_jam_configs", 1, misc_jam_method); });
    registerRoute("/logo_on", []() { storeEEPROMAndSet("logo_configs", 0, logo); });
    registerRoute("/logo_off", []() { storeEEPROMAndSet("logo_configs", 1, logo); });
    registerRoute("/enable_display", []() { storeEEPROMAndReset("display_configs", 0, display_setting); });
    registerRoute("/disable_display", []() { storeEEPROMAndReset("display_configs", 1, display_setting); });
    registerRoute("/button_method_0", []() { storeEEPROMAndSet("buttons_configs", 0, buttons); });
    registerRoute("/button_method_1", []() { storeEEPROMAndSet("buttons_configs", 1, buttons); });
    registerRoute("/button_method_2", []() { storeEEPROMAndSet("buttons_configs", 2, buttons); });
    registerRoute("/nrf_pa_0", []() { storeEEPROMAndSet("PA_configs", 0, nrf_pa); });
    registerRoute("/nrf_pa_1", []() { storeEEPROMAndSet("PA_configs", 1, nrf_pa); });
    registerRoute("/nrf_pa_2", []() { storeEEPROMAndSet("PA_configs", 2, nrf_pa); });
    registerRoute("/nrf_pa_3", []() { storeEEPROMAndSet("PA_configs", 3, nrf_pa); });
    registerRoute("/access_point_off", []() { access_poin_off(); });
    registerRoute("/set_nrf24_pins", []() { handlernRF24Pins(); });
    registerRoute("/setting_array", []() { handlerSSIDsArray(); });

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
  display.setTextColor(WHITE);
  display.setTextSize(1);
  if ( display_setting )
  {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
  display.clearDisplay();
  if ( logo == 0 )
  {
    display.drawBitmap(0, 0, bitmap_logo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
  display.drawBitmap(0, 0, bitmap_bluetooth_jammer, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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
  const uint8_t *bitmap = (menu_number == 0)   ? bitmap_bluetooth_jam
                          : (menu_number == 1) ? bitmap_drone_jam
                          : (menu_number == 2) ? bitmap_wifi_all
                          : (menu_number == 4) ? bitmap_zigbee_jam
                          : (menu_number == 6) ? bitmap_pls_reboot
                                               : NULL;

  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();

  switch ( menu_number )
  {
  case 0:
    jamHandler(bluetooth_jam, String("Bluetooth Jamming"), bitmap_bluetooth_jam, true, false);
    break;
  case 1:
    jamHandler(drone_jam, String("Drone Jamming"), bitmap_drone_jam, true, false);
    break;
  case 2:
    wifi_select();
    break;
  case 4:
    jamHandler(zigbee_jam, String("Zigbee Jamming"), bitmap_zigbee_jam, false, false);
    break;
  case 6:
    storeEEPROMAndSet("AP_configs", 0, logo);
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
      menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
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
      menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
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
      menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
      updateDisplay(menu_number);
    }

    if ( btnPrevious.isSingle() )
    {
      menu_number = (menu_number - 1 + (access_point == 0 ? 6 : 7)) %
                    (access_point == 0 ? 6 : 7);
      updateDisplay(menu_number);
    }
  }
}