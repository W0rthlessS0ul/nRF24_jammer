#include "bitmap.h"
#include "html.h"
#include "jam.h"
#include "options.h"
#include "serial.h"

void handleRoot() {
  String main_html = FPSTR(html);
  switch (bluetooth_jam_method)
  {
    case 0: main_html.replace("[||]EdItAbLe TeXt[||]", "21 channels"); break;
    case 1: main_html.replace("[||]EdItAbLe TeXt[||]", "80 channels"); break;
    case 2: main_html.replace("[||]EdItAbLe TeXt[||]", "80 channels"); break;
  }
  if (nrf24_count <= 0) {
    main_html.replace("if (false) { //IsModulesConfigured", "if (true) { //IsModulesConfigured");
  }

  server.send(200, "text/html", main_html.c_str());

  if (nrf24_count <= 0) {
    main_html.replace("if (true) { //IsModulesConfigured", "if (false) { //IsModulesConfigured");
  }
  switch (bluetooth_jam_method)
  {
    case 0: main_html.replace("21 channels", "[||]EdItAbLe TeXt[||]"); break;
    case 1: main_html.replace("80 channels", "[||]EdItAbLe TeXt[||]"); break;
    case 2: main_html.replace("80 channels", "[||]EdItAbLe TeXt[||]"); break;
  }
}

void HandleWebCommand(){
  String command = server.arg("command");
  command.trim();
  command.toLowerCase();
  CommandsHandler(command, true);
}

void updateDisplay(int menuNum) {
  display.clearDisplay();
  const uint8_t *bitmap = (menu_number == 0)   ? bitmap_bluetooth_jammer
                          : (menu_number == 1) ? bitmap_drone_jammer
                          : (menu_number == 2) ? bitmap_wifi_jammer
                          : (menu_number == 3) ? bitmap_ble_jammer
                          : (menu_number == 4) ? bitmap_zigbee_jammer
                          : (menu_number == 5) ? bitmap_misc_jammer
                                               : bitmap_access_point;
  display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  display.display();
}

void handlernRF24Pins() {
  nrf24_count = server.arg("count").toInt();
  String ceStr = server.arg("ce");
  String csnStr = server.arg("csn");
  auto SplitStr = [](const String &str, int *arr, int maxCount) {
    int idx = 0;
    int start = 0;
    while (idx < maxCount) {
      int sep = str.indexOf('|', start);
      if (sep == -1)
        sep = str.length();
      String part = str.substring(start, sep);
      arr[idx++] = part.toInt();
      if (sep == str.length())
        break;
      start = sep + 1;
    }
  };
  SplitStr(ceStr, ce_pins, nrf24_count);
  SplitStr(csnStr, csn_pins, nrf24_count);
  for (int i = 0; i < nrf24_count; i++) {
    EEPROM.write(74 + i, ce_pins[i]);
    EEPROM.write(104 + i, csn_pins[i]);
  }
  EEPROM.write(134, nrf24_count);
  EEPROM.commit();
  handleRoot();
}

void handlenRF24Init() {
  nrf24_count = EEPROM.read(134);
  if (nrf24_count > 30) {
    nrf24_count = 0;
    return;
  }
  for (int i = 0; i < nrf24_count; i++) {
    ce_pins[i] = EEPROM.read(74 + i);
    csn_pins[i] = EEPROM.read(104 + i);
  }
}

void WiFiChannelHandler() {
  String html = FPSTR(html_wifi_channel);
  for (int channel = 1; channel <= 13; channel++) {
    html.replace("let ch"+String(channel)+" = 0;", "let ch"+String(channel)+" = "+String(WiFiScanChannels[channel-1])+";");
  }
  server.send(200, "text/html", html.c_str());
  for (int channel = 1; channel <= 13; channel++) {
    html.replace("let ch"+String(channel)+" = "+String(WiFiScanChannels[channel-1])+";", "let ch"+String(channel)+" = 0;");
  }
}

void RescanHandler(){
  scan_wifi_channels(WiFiScanChannels, false);
  handleRoot();
}

void sendHtmlAndExecute(const char *htmlResponse, void (*action)() = nullptr) {
  server.send(200, "text/html", htmlResponse);
  delay(500);
  if (action)
    action();
}

void jamHandler(String htmlResponse, void (*jamFunction)(),
                const unsigned char *bitmap) {
  String html = FPSTR(html_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  display.display();
  html.replace("[||]EdItAbLe TeXt[||]", htmlResponse);
  sendHtmlAndExecute(html.c_str(), jamFunction);
  html.replace(htmlResponse, "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void miscChannelsHandler() {
  int channel1 = server.arg("start").toInt();
  int channel2 = server.arg("stop").toInt();
  String html = FPSTR(html_jam);
  html.replace("[||]EdItAbLe TeXt[||]", "Jamming from "+String(channel1)+" to "+String(channel2));
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
  misc_jam(channel1, channel2);
  html.replace("Jamming from "+String(channel1)+" to "+String(channel2), "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void wifiChannelsHandler() {
  int channel = server.arg("channel").toInt();

  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
  display.display();
  String html = FPSTR(html_jam);
  html.replace("[||]EdItAbLe TeXt[||]", "Jamming "+String(channel)+" WiFi Channel");
  sendHtmlAndExecute(html.c_str());
  wifi_channel(channel);
  html.replace("Jamming "+String(channel)+" WiFi Channel", "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void nRF24SettingsHandler(){
  String html = FPSTR(html_nrf24_settings);
  String modulesScript = "<script>window.currentModules = [";
  for (int i = 0; i < nrf24_count; i++) {
      modulesScript += "{ce: " + String(ce_pins[i]) + ", csn: " + String(csn_pins[i]) + "}";
      if (i < nrf24_count - 1) {
          modulesScript += ",";
      }
  }
  modulesScript += "];</script>";
  html.replace("</body>", modulesScript + "</body>");
  sendHtmlAndExecute(html.c_str());
}

void settingsHandler(String htmlResponse, int index, bool editable, int SettingNumber) {
  switch (SettingNumber)
  {
    case 0: break;
    case 1: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_buttons_settings); break;
    case 2: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_logo_settings); break;
    case 3: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_misc_settings); break;
    case 4: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_display_settings); break;
    case 5: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_bluetooth_settings); break;
    case 6: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_drone_settings); break;
    case 7: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_separate_or_together); break;
    case 8: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_nrf_pa_settings); break;
  }
  if (index == 1010110){
    htmlResponse.replace("<div class=\"version-badge\">version</div>", "<div class=\"version-badge\">"+Version_Number+"</div>");
  }
  else if (editable)
    htmlResponse.replace("<!-- "+String(index)+" --><button class=\"btn\"", "<!-- "+String(index)+" --><button class=\"btn btn_installed\"");

  sendHtmlAndExecute(htmlResponse.c_str());

  switch (SettingNumber)
  {
    case 0: break;
    case 1: htmlResponse.replace(html_buttons_settings, "[||]EdItAbLe TeXt[||]"); break;
    case 2: htmlResponse.replace(html_logo_settings, "[||]EdItAbLe TeXt[||]"); break;
    case 3: htmlResponse.replace(html_misc_settings, "[||]EdItAbLe TeXt[||]"); break;
    case 4: htmlResponse.replace(html_display_settings, "[||]EdItAbLe TeXt[||]"); break;
    case 5: htmlResponse.replace(html_bluetooth_settings, "[||]EdItAbLe TeXt[||]"); break;
    case 6: htmlResponse.replace(html_drone_settings, "[||]EdItAbLe TeXt[||]"); break;
    case 7: htmlResponse.replace(html_separate_or_together, "[||]EdItAbLe TeXt[||]"); break;
    case 8: htmlResponse.replace(html_nrf_pa_settings, "[||]EdItAbLe TeXt[||]"); break;
  }
  if (index == 1010110){
    htmlResponse.replace("<div class=\"version-badge\">"+Version_Number+"</div>", "<div class=\"version-badge\">version</div>");
  }
  else if (editable)
    htmlResponse.replace("<!-- "+String(index)+" --><button class=\"btn btn_installed\"", "<!-- "+String(index)+" --><button class=\"btn\"");
}

void storeEEPROMAndReset(int index, int value, int &targetVar){
  settingsHandler(html_pls_reboot, 0, false, 0);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 32, WHITE);
  display.display();
  EEPROM.write(index, value);
  EEPROM.commit();
  targetVar = value;
  handleRoot();
  ESP.restart();
}

void storeEEPROMAndSet(int index, int value, int &targetVar) {
  EEPROM.write(index, value);
  EEPROM.commit();
  targetVar = value;
  handleRoot();
}

void registerRoute(const char *path, void (*handler)()) {
  server.on(path, handler);
}

void saveWiFiSettings(const char *new_ssid, const char *new_password) {
  for (int i = 0; i < 32; i++) {
    if (i < strlen(new_ssid)) {
      EEPROM.write(11 + i, new_ssid[i]);
    } else {
      EEPROM.write(11 + i, 0);
    }
  }

  for (int i = 0; i < 32; i++) {
    if (i < strlen(new_password)) {
      EEPROM.write(43 + i, new_password[i]);
    } else {
      EEPROM.write(43 + i, 0);
    }
  }

  EEPROM.commit();
}

void initWiFiSettings() {
  if (EEPROM.read(11) == 255) {
    saveWiFiSettings(default_ssid, default_password);
  }
}

String getSSIDFromEEPROM() {
  char ssid[33] = {0};
  for (int i = 0; i < 32; i++) {
    ssid[i] = EEPROM.read(11 + i);
    if (ssid[i] == 0)
      break;
  }
  return String(ssid);
}

String getPasswordFromEEPROM() {
  char password[33] = {0};
  for (int i = 0; i < 32; i++) {
    password[i] = EEPROM.read(43 + i);
    if (password[i] == 0)
      break;
  }
  return String(password);
}

void handleSaveWiFiSettings() {
  String new_ssid = server.arg("ssid");
  String new_password = server.arg("password");

  if (new_ssid == "" && new_password == "") {
    server.send(200, "text/html", html_pls_reboot);
    storeEEPROMAndSet(8, 1, access_point);
    return;
  }

  saveWiFiSettings(new_ssid.c_str(), new_password.c_str());

  server.send(200, "text/html", html_pls_reboot);
  delay(1000);
  ESP.restart();
}

void handleResetWiFiSettings() {
  saveWiFiSettings(default_ssid, default_password);

  server.send(200, "text/html", html_pls_reboot);
  delay(1000);
  ESP.restart();
}

void handleFileUpload() {
  HTTPUpload &upload = server.upload();
  static unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 10;

  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
      return;
    } else {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      lastUpdate = millis();
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }

    if (millis() - lastUpdate >= updateInterval) {
      lastUpdate = millis();
    }
  } else if (upload.status == UPLOAD_FILE_END) {
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    Update.abort();
    Serial.println("Update aborted");
  }
}

void misc() {
  int flag = 0;
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
    if (channel > 125) {
      channel = 0;
    }
  };
  auto reduceChannel = [&](int &channel) {
    channel--;
    if (channel < 0) {
      channel = 125;
    }
  };

  while (true) {
    butt1.tick();
    buttNext.tick();
    buttPrevious.tick();
    if (buttons == 0) {
      if (butt1.isSingle() || butt1.isHold()) {
        if (butt1.isHold()) {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      } else if (butt1.isDouble()) {
        if (flag == 0) {
          flag = 1;
        } else {
          if (channel1 > channel2) {
            display_info("Error: Second < First");
            flag = 0;
          } else {
            display_info("Jamming Started");
            misc_jam(channel1, channel2);
            break;
          }
        }
      }
    }
    if (buttons == 1) {
      if (buttNext.isSingle() || buttNext.isHold()) {
        if (buttNext.isHold()) {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      } else if (butt1.isSingle()) {
        if (flag == 0) {
          flag = 1;
        } else {
          if (channel1 > channel2) {
            display_info("Error: Second < First");
            flag = 0;
          } else {
            display_info("Jamming Started");
            misc_jam(channel1, channel2);
            break;
          }
        }
      }
    }
    if (buttons == 2) {
      if (buttNext.isSingle() || buttNext.isHold()) {
        if (buttNext.isHold()) {
          delay(100);
        }
        incrementChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      }
      if (buttPrevious.isSingle() || buttPrevious.isHold()) {
        if (buttPrevious.isHold()) {
          delay(100);
        }
        reduceChannel(flag == 0 ? channel1 : channel2);
        display_info("");
      } else if (butt1.isSingle()) {
        if (flag == 0) {
          flag = 1;
        } else {
          if (channel1 > channel2) {
            display_info("Error: Second < First");
            flag = 0;
          } else {
            display_info("Jamming Started");
            misc_jam(channel1, channel2);
            break;
          }
        }
      }
    }
  }
}

void wifi_select() {
  auto display_info = [&](int flag, int wifi_points) -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("channel: " + String(flag));
    display.setCursor(0, 10);
    display.println("Wi-Fi APs: " + String(wifi_points));
    display.display();
  };
  auto scan_wifi = [&](int &channelCount, int *WiFi_channels) -> void {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi APs");
    display.display();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int networks = scan_wifi_channels(WiFiScanChannels, false);
    display.setCursor(0, 10);
    display.println("Finded " + String(networks) + " APs");
    display.display();
    String current_ssid = getSSIDFromEEPROM();
    String current_password = getPasswordFromEEPROM();
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
    delay(1000);
  };
  int channelCount = 0;
  int menu_number = 0;

  while (true) {
    butt1.tick();
    buttNext.tick();
    buttPrevious.tick();
    if (buttons == 0) {
      if (butt1.isSingle()) {
        menu_number = (menu_number + 1) % 3;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0) ? bitmap_wifi_all :
            (menu_number == 1) ? bitmap_wifi_select : bitmap_smart_jammer;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
      }
      if (butt1.isHolded()) {
        if (menu_number == 0) {
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          wifi_jam();
          return;
        }
        else if (menu_number == 1) {
          
          scan_wifi(channelCount, WiFiScanChannels);

          flag = 1;
          display_info(flag, WiFiScanChannels[flag - 1]);
          while (true) {
            butt1.tick();
            buttNext.tick();
            buttPrevious.tick();
            if (butt1.isSingle()) {
              flag++;
              if (flag > 14) {
                flag = 1;
              }
              display_info(flag, WiFiScanChannels[flag - 1]);
            }
            if (butt1.isHolded()) {
              display.clearDisplay();
              display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
              display.display();
              wifi_channel(flag);
              return;
            }
          }
        }
        else if (menu_number == 2){
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          wifi_scan_jam();
          return;
        }
      }
    }
    if (buttons == 1) {
      if (buttNext.isSingle()) {
        menu_number = (menu_number + 1) % 3;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0) ? bitmap_wifi_all :
            (menu_number == 1) ? bitmap_wifi_select : bitmap_smart_jammer;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
      }
      if (butt1.isSingle()) {
        if (menu_number == 0) {
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          wifi_jam();
          return;
        }
        else if (menu_number == 1) {
          
          scan_wifi(channelCount, WiFiScanChannels);

          flag = 1;
          display_info(flag, WiFiScanChannels[flag - 1]);
          while (true) {
            butt1.tick();
            buttNext.tick();
            buttPrevious.tick();
            if (buttNext.isSingle()) {
              flag++;
              if (flag > 14) {
                flag = 1;
              }
              display_info(flag, WiFiScanChannels[flag - 1]);
            }
            if (butt1.isSingle()) {
              display.clearDisplay();
              display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
              display.display();
              wifi_channel(flag);
              return;
            }
          }
        }
        else if (menu_number == 2){
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          wifi_scan_jam();
          return;
        }
      }
    }
    if (buttons == 2) {
      if (buttNext.isSingle()) {
        menu_number = (menu_number + 1) % 3;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0) ? bitmap_wifi_all :
            (menu_number == 1) ? bitmap_wifi_select : bitmap_smart_jammer;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
      }
      if (buttPrevious.isSingle()) {
        menu_number = (menu_number - 1 + 3) % 3;
        display.clearDisplay();
        const uint8_t *bitmap =
            (menu_number == 0) ? bitmap_wifi_all :
            (menu_number == 1) ? bitmap_wifi_select : bitmap_smart_jammer;
        display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
        display.display();
      }

      if (butt1.isSingle()) {
        if (menu_number == 0) {
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          wifi_jam();
          return;
        }
        else if (menu_number == 1) {
          
          scan_wifi(channelCount, WiFiScanChannels);

          flag = 1;
          display_info(flag, WiFiScanChannels[flag - 1]);
          while (true) {
            butt1.tick();
            buttNext.tick();
            buttPrevious.tick();
            if (buttNext.isSingle()) {
              flag++;
              if (flag > 14) {
                flag = 1;
              }
              display_info(flag, WiFiScanChannels[flag - 1]);
            }
            if (buttPrevious.isSingle()) {
              flag = flag - 1;
              if (flag < 1) {
                flag = 14;
              }
              if (flag > 14) {
                flag = 1;
              }
              display_info(flag, WiFiScanChannels[flag - 1]);
            }
            if (butt1.isSingle()) {
              display.clearDisplay();
              display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
              display.display();
              wifi_channel(flag);
              return;
            }
          }
        }
        else if (menu_number == 2){
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          wifi_scan_jam();
          return;
        }
      }
    }
  }
}

void access_poin_off() {
  settingsHandler(html_pls_reboot, 0, false, 0);
  storeEEPROMAndSet(8, 1, access_point);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 32, WHITE);
  display.display();
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  for (int i = 0; i < 11; i++) {
    if (EEPROM.read(i) == 255) {
      EEPROM.write(i, 0);
    }
  }

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  handlenRF24Init();

  initWiFiSettings();

  bluetooth_jam_method = EEPROM.read(0);
  drone_jam_method = EEPROM.read(1);
  display_setting = EEPROM.read(2);
  wifi_jam_method = EEPROM.read(3);
  nrf_pa = EEPROM.read(5);
  misc_jam_method = EEPROM.read(6);
  logo = EEPROM.read(7);
  access_point = EEPROM.read(8);
  buttons = EEPROM.read(9);
  Separate_or_together = EEPROM.read(10);

  if (access_point == 0) {
    String current_ssid = getSSIDFromEEPROM();
    String current_password = getPasswordFromEEPROM();

    WiFi.softAP(current_ssid.c_str(), current_password.c_str());

    dnsServer.start(53, "*", WiFi.softAPIP());

    registerRoute("/", handleRoot);
    registerRoute("/bluetooth_jam", []() { jamHandler(String("Bluetooth Jamming"), bluetooth_jam, bitmap_bluetooth_jam); });
    registerRoute("/drone_jam", []() { jamHandler(String("Drone Jamming"), drone_jam, bitmap_drone_jam); });
    registerRoute("/wifi_jam", []() { jamHandler(String("WiFi Jamming"), wifi_jam, bitmap_wifi_jam); });
    registerRoute("/wifi_scan_jam", []() { jamHandler(String("WiFi Jamming"), wifi_scan_jam, bitmap_wifi_jam); });
    registerRoute("/ble_jam", []() { jamHandler(String("BLE Jamming"), ble_jam, bitmap_ble_jam); });
    registerRoute("/zigbee_jam", []() { jamHandler(String("Zigbee Jamming"), zigbee_jam, bitmap_zigbee_jam); });
    registerRoute("/misc_jammer", []() { sendHtmlAndExecute(html_misc_jammer); });
    registerRoute("/web_serial", []() { sendHtmlAndExecute(html_webserial); });
    registerRoute("/misc_jam", miscChannelsHandler);
    registerRoute("/wifi_selected_jam", wifiChannelsHandler);
    registerRoute("/rescan", RescanHandler);

    registerRoute("/setting_display", []() { settingsHandler(html_settings, display_setting, true, 4); });
    registerRoute("/setting_bluetooth_jam", []() { settingsHandler(html_settings, bluetooth_jam_method, true, 5); });
    registerRoute("/setting_drone_jam", []() { settingsHandler(html_settings, drone_jam_method, true, 6); });
    registerRoute("/setting_separate_together", []() { settingsHandler(html_settings, Separate_or_together, true, 7); });
    registerRoute("/setting_misc_jam", []() { settingsHandler(html_settings, misc_jam_method, true, 3); });
    registerRoute("/setting_logo", []() { settingsHandler(html_settings, logo, true, 2); });
    registerRoute("/setting_buttons", []() { settingsHandler(html_settings, buttons, true, 1); });
    registerRoute("/setting_nrf_pa", []() { settingsHandler(html_settings, nrf_pa, true, 8); });
    
    registerRoute("/OTA", []() { settingsHandler(html_ota, 1010110, false, 0); });
    registerRoute("/wifi_select", []() { settingsHandler(html_wifi_select, 0, false, 0); });
    registerRoute("/wifi_channel", WiFiChannelHandler);
    registerRoute("/wifi_settings", []() { settingsHandler(html_wifi_settings, wifi_jam_method, true, 0); });
    registerRoute("/nrf24_settings", []() { nRF24SettingsHandler(); });

    registerRoute("/WebCommand", HandleWebCommand);

    registerRoute("/generate_204", handleRoot);
    registerRoute("/redirect", handleRoot);
    registerRoute("/hotspot-detect.html", handleRoot);
    registerRoute("/canonical.html", handleRoot);
    registerRoute("/success.txt", handleRoot);
    registerRoute("/ncsi.txt", handleRoot);
    registerRoute("/connecttest.txt", handleRoot);

    server.on(
          "/update", HTTP_POST,
          []() {
            if (Update.end(true)) {
              Serial.printf("Update Success: %u bytes\n", Update.size());
              server.send(200, "text/plain", "Update Success");
              delay(100);
              display.clearDisplay();
              display.drawBitmap(0, 0, bitmap_pls_reboot, 128, 32, WHITE);
              display.display();
              delay(2000);
              ESP.restart();
            } else {
              Update.printError(Serial);
              server.send(500, "text/plain", "Update Failed");
            }
          },
          handleFileUpload);
    
    server.on("/save_wifi_settings", handleSaveWiFiSettings);
    server.on("/reset_wifi_settings", handleResetWiFiSettings);

    registerRoute("/bluetooth_method_0", []() { storeEEPROMAndSet(0, 0, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_1", []() { storeEEPROMAndSet(0, 1, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_2", []() { storeEEPROMAndSet(0, 2, bluetooth_jam_method); });
    registerRoute("/drone_method_0", []() { storeEEPROMAndSet(1, 0, drone_jam_method); });
    registerRoute("/drone_method_1", []() { storeEEPROMAndSet(1, 1, drone_jam_method); });
    registerRoute("/separate_or_together_method_0", []() { storeEEPROMAndSet(4, 0, Separate_or_together); });
    registerRoute("/separate_or_together_method_1", []() { storeEEPROMAndSet(4, 1, Separate_or_together); });
    registerRoute("/misc_method_0", []() { storeEEPROMAndSet(6, 0, misc_jam_method); });
    registerRoute("/misc_method_1", []() { storeEEPROMAndSet(6, 1, misc_jam_method); });
    registerRoute("/logo_on", []() { storeEEPROMAndSet(7, 0, logo); });
    registerRoute("/logo_off", []() { storeEEPROMAndSet(7, 1, logo); });
    registerRoute("/enable_display", []() { storeEEPROMAndReset(2, 0, display_setting); });
    registerRoute("/disable_display", []() { storeEEPROMAndReset(2, 1, display_setting); });
    registerRoute("/button_method_0", []() { storeEEPROMAndSet(9, 0, buttons); });
    registerRoute("/button_method_1", []() { storeEEPROMAndSet(9, 1, buttons); });
    registerRoute("/button_method_2", []() { storeEEPROMAndSet(9, 2, buttons); });
    registerRoute("/nrf_pa_0", []() { storeEEPROMAndSet(5, 0, nrf_pa); });
    registerRoute("/nrf_pa_1", []() { storeEEPROMAndSet(5, 1, nrf_pa); });
    registerRoute("/nrf_pa_2", []() { storeEEPROMAndSet(5, 2, nrf_pa); });
    registerRoute("/nrf_pa_3", []() { storeEEPROMAndSet(5, 3, nrf_pa); });
    registerRoute("/access_point_off", []() { access_poin_off(); });
    registerRoute("/set_nrf24_pins", []() { handlernRF24Pins(); });

    server.begin();
  }

  Serial.println(logotype+"\n\n");

  Serial.println("help ==> Displays a list of available commands\n");

  butt1.setClickTimeout(200);
  buttNext.setClickTimeout(200);
  buttPrevious.setClickTimeout(200);
  butt1.setTimeout(600);
  buttNext.setTimeout(600);
  buttPrevious.setTimeout(600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  if (display_setting){
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
  display.clearDisplay();
  if (logo == 0) {
    display.drawBitmap(0, 0, bitmap_logo, 128, 32, WHITE);
  }
  else {
    display.setCursor(0, 0);
    display.println("Scanning Wi-Fi APs");
  }
  display.display();

  int networks = scan_wifi_channels(WiFiScanChannels, false);

  if (logo != 0){
    display.setCursor(0, 10);
    display.println("Finded " + String(networks) + " APs");
    display.display();
    delay(1000);
  }

  display.setCursor(0, 0);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_bluetooth_jammer, 128, 32, WHITE);
  display.display();
}

void executeAction(int menuNum) {
  if (nrf24_count <= 0) {
    display.clearDisplay();
    display.setCursor(40, 0);
    display.println("WARNING");

    display.setCursor(5, 10);
    display.println("nRF24 not configured");

    display.setCursor(7, 20);
    display.println("Configure in web UI");

    display.display();
    while (nrf24_count <= 0) {
      server.handleClient();
      dnsServer.processNextRequest();
      delay(100);
    }
  }
  if (menuNum == 5) {
    misc();
    updateDisplay(menu_number);
    return;
  } 

  display.clearDisplay();
  const uint8_t *bitmap = (menu_number == 0)   ? bitmap_bluetooth_jam
                          : (menu_number == 1) ? bitmap_drone_jam
                          : (menu_number == 2) ? bitmap_wifi_all
                          : (menu_number == 3) ? bitmap_ble_jam
                          : (menu_number == 4) ? bitmap_zigbee_jam
                          : (menu_number == 6) ? bitmap_pls_reboot
                                               : NULL;

  display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
  display.display();

  switch (menu_number) {
    case 0:
      bluetooth_jam();
      break;
    case 1:
      drone_jam();
      break;
    case 2:
      wifi_select();
      break;
    case 3:
      ble_jam();
      break;
    case 4:
      zigbee_jam();
      break;
    case 6:
      storeEEPROMAndSet(8, 0, logo);
      break;
    default:
      break;
  }
  if (menuNum != 6)
    updateDisplay(menuNum);
}

void loop() {
  butt1.tick();
  buttNext.tick();
  buttPrevious.tick();

  SerialCommands();

  if (access_point == 0) {
    server.handleClient();
    dnsServer.processNextRequest();
  }
  if (buttons == 0) {
    if (butt1.isSingle()) {
      menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
      updateDisplay(menu_number);
    }
    if (butt1.isHolded()) {
      executeAction(menu_number);
      butt1.resetStates();
    }
  } else if (buttons == 1) {
    if (butt1.isSingle()) {
      executeAction(menu_number);
    }
    if (buttNext.isSingle()) {
      menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
      updateDisplay(menu_number);
    }
  } else if (buttons == 2) {
    if (butt1.isSingle()) {
      executeAction(menu_number);
    }

    if (buttNext.isSingle()) {
      menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7);
      updateDisplay(menu_number);
    }

    if (buttPrevious.isSingle()) {
      menu_number = (menu_number - 1 + (access_point == 0 ? 6 : 7)) %
                    (access_point == 0 ? 6 : 7);
      updateDisplay(menu_number);
    }
  }
}