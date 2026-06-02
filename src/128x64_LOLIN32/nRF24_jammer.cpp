// ============================================================
// nRF24_jammer.cpp - Adattato per LOLIN32 Lite
// Modifiche rispetto all'originale 128x64_Flexible:
//   1. Wire.begin(OLED_SDA, OLED_SCL) prima di display.begin()
//   2. pinMode(LED_PIN, OUTPUT) + LED_OFF all'avvio
//   3. Auto-init CE/CSN di default alla prima accensione
//   4. Tutti gli include puntano a config.h LOLIN32
// ============================================================
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
  case 0: main_html.replace("[||]EdItAbLe TeXt[||]", "21 channels"); break;
  case 1: main_html.replace("[||]EdItAbLe TeXt[||]", "80 channels"); break;
  case 2: main_html.replace("[||]EdItAbLe TeXt[||]", "80 channels"); break;
  }
  if ( nrf24_count <= 0 )
    main_html.replace("if (false) { //IsModulesConfigured", "if (true) { //IsModulesConfigured");
  server.send(200, "text/html", main_html.c_str());
  if ( nrf24_count <= 0 )
    main_html.replace("if (true) { //IsModulesConfigured", "if (false) { //IsModulesConfigured");
  switch ( bluetooth_jam_method )
  {
  case 0: main_html.replace("21 channels", "[||]EdItAbLe TeXt[||]"); break;
  case 1: main_html.replace("80 channels", "[||]EdItAbLe TeXt[||]"); break;
  case 2: main_html.replace("80 channels", "[||]EdItAbLe TeXt[||]"); break;
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
  const uint8_t *bitmap =
      (menu_number == 0) ? bitmap_bluetooth_jammer
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
    int idx = 0, start = 0;
    while ( idx < maxCount )
    {
      int sep = str.indexOf('|', start);
      if ( sep == -1 ) sep = str.length();
      arr[idx++] = str.substring(start, sep).toInt();
      if ( sep == str.length() ) break;
      start = sep + 1;
    }
  };
  SplitStr(ceStr, ce_pins, nrf24_count);
  SplitStr(csnStr, csn_pins, nrf24_count);
  for ( int i = 0; i < nrf24_count; i++ )
  {
    EEPROM.write(74 + i, ce_pins[i]);
    EEPROM.write(104 + i, csn_pins[i]);
  }
  EEPROM.write(134, nrf24_count);
  EEPROM.commit();
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handlenRF24Init()
{
  nrf24_count = EEPROM.read(134);
  if ( nrf24_count > 30 )
  {
    nrf24_count = 0;
    return;
  }
  for ( int i = 0; i < nrf24_count; i++ )
  {
    ce_pins[i]  = EEPROM.read(74 + i);
    csn_pins[i] = EEPROM.read(104 + i);
  }
}

void WiFiChannelHandler()
{
  String html      = FPSTR(html_wifi_channel);
  String ssidsJson = "[";
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String escaped = APs_array[ch - 1];
    escaped.replace("\\", "\\\\"); escaped.replace("\"", "\\\""); escaped.replace("\n", "\\n");
    ssidsJson += "\"" + escaped + "\"";
    if ( ch < 14 ) ssidsJson += ",";
  }
  ssidsJson += "]";
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String search  = "let ch" + String(ch) + " = 0;";
    String replace = "let ch" + String(ch) + " = " + String(WiFiScanChannels[ch - 1]) + ";";
    html.replace(search, replace);
  }
  html.replace("let ssidsByChannel = [];", "let ssidsByChannel = " + ssidsJson + ";");
  server.send(200, "text/html", html.c_str());
}

void WiFiDeauthChannelHandler()
{
  String html      = FPSTR(html_wifi_channel);
  String ssidsJson = "[";
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String escaped = APs_array[ch - 1];
    escaped.replace("\\", "\\\\"); escaped.replace("\"", "\\\""); escaped.replace("\n", "\\n");
    ssidsJson += "\"" + escaped + "\"";
    if ( ch < 14 ) ssidsJson += ",";
  }
  ssidsJson += "]";
  for ( int ch = 1; ch <= 14; ch++ )
  {
    String search  = "let ch" + String(ch) + " = 0;";
    String replace = "let ch" + String(ch) + " = " + String(WiFiScanChannels[ch - 1]) + ";";
    html.replace(search, replace);
  }
  html.replace("let ssidsByChannel = [];", "let ssidsByChannel = " + ssidsJson + ";");
  html.replace("/wifi_selected_jam", "/wifi_selected_deauth");
  server.send(200, "text/html", html.c_str());
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
  if ( action ) action();
}

void jamHandler(String htmlResponse, void (*jamFunction)(), const unsigned char *bitmap)
{
  String html = FPSTR(html_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  html.replace("[||]EdItAbLe TeXt[||]", htmlResponse);
  sendHtmlAndExecute(html.c_str(), jamFunction);
  html.replace(htmlResponse, "[||]EdItAbLe TeXt[||]");
  updateDisplay(menu_number);
}

void jamScanHandler(String htmlResponse, void (*jamFunction)(), const unsigned char *bitmap)
{
  String html = FPSTR(html_jam);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  html.replace("[||]EdItAbLe TeXt[||]", htmlResponse);
  server.send(200, "text/html", html);
  html.replace(htmlResponse, "[||]EdItAbLe TeXt[||]");
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  NumberChannels = scan_wifi_APs(WiFiScanChannels, true);
  jamFunction();
  updateDisplay(menu_number);
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

void miscChannelsHandler()
{
  int    channel1 = server.arg("start").toInt();
  int    channel2 = server.arg("stop").toInt();
  String html     = FPSTR(html_jam);
  html.replace("[||]EdItAbLe TeXt[||]", "Jamming from " + String(channel1) + " to " + String(channel2));
  sendHtmlAndExecute(html.c_str());
  misc_jam(channel1, channel2);
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
  wifi_channel(channel - 1);
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
  updateDisplay(menu_number);
}

void nRF24SettingsHandler()
{
  String html          = FPSTR(html_nrf24_settings);
  String modulesScript = "<script>window.currentModules = [";
  for ( int i = 0; i < nrf24_count; i++ )
  {
    modulesScript += "{ce: " + String(ce_pins[i]) + ", csn: " + String(csn_pins[i]) + "}";
    if ( i < nrf24_count - 1 ) modulesScript += ",";
  }
  modulesScript += "];</script>";
  html.replace("</body>", modulesScript + "</body>");
  sendHtmlAndExecute(html.c_str());
}

void settingsHandler(String htmlResponse, int index, bool editable, int SettingNumber)
{
  switch ( SettingNumber )
  {
  case 1: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_buttons_settings);  break;
  case 2: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_logo_settings);     break;
  case 3: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_misc_settings);     break;
  case 4: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_display_settings);  break;
  case 5: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_bluetooth_settings);break;
  case 6: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_drone_settings);    break;
  case 7: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_separate_or_together); break;
  case 8: htmlResponse.replace("[||]EdItAbLe TeXt[||]", html_nrf_pa_settings);   break;
  default: break;
  }
  if ( index == 1010110 )
    htmlResponse.replace("<div class=\"version-badge\">version</div>",
                         "<div class=\"version-badge\">" + String(Version_Number) + "</div>");
  else if ( editable )
    htmlResponse.replace("<!-- " + String(index) + " --><button class=\"btn\"",
                         "<!-- " + String(index) + " --><button class=\"btn btn_installed\"");
  sendHtmlAndExecute(htmlResponse.c_str());
}

void storeEEPROMAndReset(int index, int value, int &targetVar)
{
  settingsHandler(String(FPSTR(html_pls_reboot)), 0, false, 0);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  EEPROM.write(index, value);
  EEPROM.commit();
  targetVar = value;
  ESP.restart();
}

void storeEEPROMAndSet(int index, int value, int &targetVar)
{
  EEPROM.write(index, value);
  EEPROM.commit();
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
  for ( int i = 0; i < 32; i++ )
    EEPROM.write(11 + i, i < (int)strlen(new_ssid)     ? new_ssid[i]     : 0);
  for ( int i = 0; i < 32; i++ )
    EEPROM.write(43 + i, i < (int)strlen(new_password) ? new_password[i] : 0);
  EEPROM.commit();
}

void initWiFiSettings()
{
  if ( EEPROM.read(11) == 255 )
    saveWiFiSettings(default_ssid, default_password);
}

String getSSIDFromEEPROM()
{
  char ssid[33] = {0};
  for ( int i = 0; i < 32; i++ ) { ssid[i] = EEPROM.read(11 + i); if ( !ssid[i] ) break; }
  return String(ssid);
}

String getPasswordFromEEPROM()
{
  char password[33] = {0};
  for ( int i = 0; i < 32; i++ ) { password[i] = EEPROM.read(43 + i); if ( !password[i] ) break; }
  return String(password);
}

void handleSaveWiFiSettings()
{
  String new_ssid     = server.arg("ssid");
  String new_password = server.arg("password");
  if ( new_ssid == "" && new_password == "" )
  {
    server.send_P(200, "text/html", html_pls_reboot);
    storeEEPROMAndSet(8, 1, access_point);
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
  HTTPUpload &upload = server.upload();
  if ( upload.status == UPLOAD_FILE_START )
  {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if ( !Update.begin(UPDATE_SIZE_UNKNOWN) )
      Update.printError(Serial);
  }
  else if ( upload.status == UPLOAD_FILE_WRITE )
  {
    if ( Update.write(upload.buf, upload.currentSize) != upload.currentSize )
      Update.printError(Serial);
  }
  else if ( upload.status == UPLOAD_FILE_ABORTED )
  {
    Update.abort();
    Serial.println("Update aborted");
  }
}

void access_poin_off()
{
  settingsHandler(String(FPSTR(html_pls_reboot)), 0, false, 0);
  storeEEPROMAndSet(8, 1, access_point);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_pls_reboot, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  delay(1000);
  ESP.restart();
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  // ---- LOLIN32 Lite: LED active LOW ----
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);  // Spento all'avvio

  // ---- I2C con pin espliciti (obbligatorio su ESP32) ----
  Wire.begin(OLED_SDA, OLED_SCL);  // SDA=21, SCL=22

  for ( int i = 0; i < 11; i++ )
    if ( EEPROM.read(i) == 255 ) EEPROM.write(i, 0);

  // ---- Init nRF24: auto-configura CE=4, CSN=5 alla prima accensione ----
  handlenRF24Init();
  if ( nrf24_count == 0 || nrf24_count > 30 )
  {
    nrf24_count = 1;
    ce_pins[0]  = DEFAULT_CE_PIN;   // GPIO 4
    csn_pins[0] = DEFAULT_CSN_PIN;  // GPIO 5
    EEPROM.write(74,  ce_pins[0]);
    EEPROM.write(104, csn_pins[0]);
    EEPROM.write(134, nrf24_count);
    EEPROM.commit();
    Serial.println("  nRF24 auto-configured: CE=4, CSN=5");
  }

  initWiFiSettings();

  bluetooth_jam_method = EEPROM.read(0);
  drone_jam_method     = EEPROM.read(1);
  display_setting      = EEPROM.read(2);
  wifi_jam_method      = EEPROM.read(3);
  nrf_pa               = EEPROM.read(5);
  misc_jam_method      = EEPROM.read(6);
  logo                 = EEPROM.read(7);
  access_point         = EEPROM.read(8);
  buttons              = EEPROM.read(9);
  Separate_or_together = EEPROM.read(10);

  if ( access_point == 0 )
  {
    String current_ssid     = getSSIDFromEEPROM();
    String current_password = getPasswordFromEEPROM();
    WiFi.softAP(current_ssid.c_str(), current_password.c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());

    registerRoute("/",                     handleRoot);
    registerRoute("/bluetooth_jam",        []() { jamHandler(String("Bluetooth Jamming"), bluetooth_jam, bitmap_bluetooth_jam); });
    registerRoute("/drone_jam",            []() { jamHandler(String("Drone Jamming"), drone_jam, bitmap_drone_jam); });
    registerRoute("/wifi_jam",             []() { jamHandler(String("WiFi Jamming"), wifi_jam, bitmap_wifi_jam); });
    registerRoute("/wifi_deauth_all",      []() { jamHandler(String("WiFi Deauthing"), wifi_deauth_all, bitmap_wifi_deauth); });
    registerRoute("/wifi_scan_jam",        []() { jamScanHandler(String("WiFi Jamming"), wifi_scan_jam, bitmap_wifi_jam); });
    registerRoute("/wifi_deauth_scan",     []() { jamScanHandler(String("WiFi Deauthing"), wifi_deauth_scan, bitmap_wifi_deauth); });
    registerRoute("/ble_advertising_jam",  []() { jamHandler(String("BLE Jamming"), ble_advertising_jam, bitmap_ble_jam); });
    registerRoute("/ble_data_jam",         []() { jamHandler(String("BLE Jamming"), ble_data_jam, bitmap_ble_jam); });
    registerRoute("/zigbee_jam",           []() { jamHandler(String("Zigbee Jamming"), zigbee_jam, bitmap_zigbee_jam); });
    registerRoute("/misc_jammer",          []() { sendHtmlAndExecute(html_misc_jammer); });
    registerRoute("/web_serial",           []() { sendHtmlAndExecute(html_webserial); });
    registerRoute("/misc_jam",             miscChannelsHandler);
    registerRoute("/wifi_selected_jam",    wifiChannelsHandler);
    registerRoute("/wifi_selected_deauth", wifiDeauthChannelsHandler);
    registerRoute("/rescan",               RescanHandler);

    registerRoute("/setting_display",           []() { settingsHandler(html_settings, display_setting, true, 4); });
    registerRoute("/setting_bluetooth_jam",      []() { settingsHandler(html_settings, bluetooth_jam_method, true, 5); });
    registerRoute("/setting_drone_jam",          []() { settingsHandler(html_settings, drone_jam_method, true, 6); });
    registerRoute("/setting_separate_together",  []() { settingsHandler(html_settings, Separate_or_together, true, 7); });
    registerRoute("/setting_misc_jam",           []() { settingsHandler(html_settings, misc_jam_method, true, 3); });
    registerRoute("/setting_logo",               []() { settingsHandler(html_settings, logo, true, 2); });
    registerRoute("/setting_buttons",            []() { settingsHandler(html_settings, buttons, true, 1); });
    registerRoute("/setting_nrf_pa",             []() { settingsHandler(html_settings, nrf_pa, true, 8); });
    registerRoute("/OTA",                        []() { settingsHandler(html_ota, 1010110, false, 0); });
    registerRoute("/ble_select",                 []() { settingsHandler(html_ble_select, 0, false, 0); });
    registerRoute("/wifi_select",                []() { settingsHandler(html_wifi_select, 0, false, 0); });
    registerRoute("/wifi_channel",               WiFiChannelHandler);
    registerRoute("/wifi_deauth_channel",        WiFiDeauthChannelHandler);
    registerRoute("/wifi_settings",              []() { settingsHandler(html_wifi_settings, wifi_jam_method, true, 0); });
    registerRoute("/nrf24_settings",             []() { nRF24SettingsHandler(); });
    registerRoute("/WebCommand",                 HandleWebCommand);
    registerRoute("/generate_204",               handleRoot);
    registerRoute("/redirect",                   handleRoot);
    registerRoute("/hotspot-detect.html",         handleRoot);
    registerRoute("/canonical.html",             handleRoot);
    registerRoute("/ncsi.txt",                   handleRoot);

    server.on("/update", HTTP_POST, []() {
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
    }, handleFileUpload);

    server.on("/save_wifi_settings",  handleSaveWiFiSettings);
    server.on("/reset_wifi_settings", handleResetWiFiSettings);

    registerRoute("/bluetooth_method_0",          []() { storeEEPROMAndSet(0, 0, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_1",          []() { storeEEPROMAndSet(0, 1, bluetooth_jam_method); });
    registerRoute("/bluetooth_method_2",          []() { storeEEPROMAndSet(0, 2, bluetooth_jam_method); });
    registerRoute("/drone_method_0",              []() { storeEEPROMAndSet(1, 0, drone_jam_method); });
    registerRoute("/drone_method_1",              []() { storeEEPROMAndSet(1, 1, drone_jam_method); });
    registerRoute("/separate_or_together_method_0", []() { storeEEPROMAndSet(4, 0, Separate_or_together); });
    registerRoute("/separate_or_together_method_1", []() { storeEEPROMAndSet(4, 1, Separate_or_together); });
    registerRoute("/misc_method_0",               []() { storeEEPROMAndSet(6, 0, misc_jam_method); });
    registerRoute("/misc_method_1",               []() { storeEEPROMAndSet(6, 1, misc_jam_method); });
    registerRoute("/logo_on",                     []() { storeEEPROMAndSet(7, 0, logo); });
    registerRoute("/logo_off",                    []() { storeEEPROMAndSet(7, 1, logo); });
    registerRoute("/enable_display",              []() { storeEEPROMAndReset(2, 0, display_setting); });
    registerRoute("/disable_display",             []() { storeEEPROMAndReset(2, 1, display_setting); });
    registerRoute("/button_method_0",             []() { storeEEPROMAndSet(9, 0, buttons); });
    registerRoute("/button_method_1",             []() { storeEEPROMAndSet(9, 1, buttons); });
    registerRoute("/button_method_2",             []() { storeEEPROMAndSet(9, 2, buttons); });
    registerRoute("/nrf_pa_0",                    []() { storeEEPROMAndSet(5, 0, nrf_pa); });
    registerRoute("/nrf_pa_1",                    []() { storeEEPROMAndSet(5, 1, nrf_pa); });
    registerRoute("/nrf_pa_2",                    []() { storeEEPROMAndSet(5, 2, nrf_pa); });
    registerRoute("/nrf_pa_3",                    []() { storeEEPROMAndSet(5, 3, nrf_pa); });
    registerRoute("/access_point_off",            []() { access_poin_off(); });
    registerRoute("/set_nrf24_pins",              []() { handlernRF24Pins(); });

    server.begin();
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

  // ---- Display init (Wire.begin gia' chiamato sopra) ----
  if ( !display.begin(SSD1306_SWITCHCAPVCC, 0x3C) )
  {
    Serial.println("SSD1306 non trovato! Verificare SDA=21, SCL=22");
    // Continua senza display
  }
  display.setTextColor(WHITE);
  display.setTextSize(1);
  if ( display_setting )
    display.ssd1306_command(SSD1306_DISPLAYOFF);

  display.clearDisplay();
  if ( logo == 0 )
    display.drawBitmap(0, 0, bitmap_logo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
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

  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_bluetooth_jammer, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
}

void executeAction(int menuNum)
{
  if ( nrf24_count <= 0 )
  {
    display.clearDisplay();
    display.setCursor(40, 0);  display.println("WARNING");
    display.setCursor(5,  10); display.println("nRF24 not configured");
    display.setCursor(7,  20); display.println("Configure in web UI");
    display.display();
    while ( nrf24_count <= 0 )
    {
      server.handleClient();
      dnsServer.processNextRequest();
      delay(100);
    }
  }
  if ( menuNum == 5 ) { misc();       updateDisplay(menu_number); return; }
  if ( menuNum == 3 ) { ble_select(); updateDisplay(menu_number); return; }

  display.clearDisplay();
  const uint8_t *bitmap =
      (menu_number == 0) ? bitmap_bluetooth_jam
    : (menu_number == 1) ? bitmap_drone_jam
    : (menu_number == 2) ? bitmap_wifi_all
    : (menu_number == 4) ? bitmap_zigbee_jam
    : (menu_number == 6) ? bitmap_pls_reboot
                         : NULL;
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();

  switch ( menu_number )
  {
  case 0: bluetooth_jam(); break;
  case 1: drone_jam();     break;
  case 2: wifi_select();   break;
  case 4: zigbee_jam();    break;
  case 6: storeEEPROMAndSet(8, 0, logo); break;
  default: break;
  }
  if ( menuNum != 6 ) updateDisplay(menuNum);
}

void loop()
{
  btnOK.tick();
  btnNext.tick();
  btnPrevious.tick();
  SerialCommands();

  if ( access_point == 0 )
  {
    server.handleClient();
    dnsServer.processNextRequest();
  }

  if ( buttons == 0 )
  {
    if ( btnOK.isSingle() )  { menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7); updateDisplay(menu_number); }
    if ( btnOK.isHolded() )  { executeAction(menu_number); btnOK.resetStates(); }
  }
  else if ( buttons == 1 )
  {
    if ( btnOK.isSingle() )   executeAction(menu_number);
    if ( btnNext.isSingle() ) { menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7); updateDisplay(menu_number); }
  }
  else if ( buttons == 2 )
  {
    if ( btnOK.isSingle() )       executeAction(menu_number);
    if ( btnNext.isSingle() )     { menu_number = (menu_number + 1) % (access_point == 0 ? 6 : 7); updateDisplay(menu_number); }
    if ( btnPrevious.isSingle() ) { menu_number = (menu_number - 1 + (access_point == 0 ? 6 : 7)) % (access_point == 0 ? 6 : 7); updateDisplay(menu_number); }
  }
}
