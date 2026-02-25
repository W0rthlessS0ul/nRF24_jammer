#include "serial.h"

String logotype = R"rawliteral(
                      GGGG        LLLL                      
                   GGGGGGGGGGGLLLLLLLLLLK                   
                  GGGGGGGGGGGGLLLLLLLLLLLL                  
                 GGGGGGGGGGGGGLLLLLLLLLLLLK                 
                 GGGGGGGGGGGGGLLLLLLLLLLLLL                 
                GGGGGGGGGGGGGGLLLLLLLLLLLLLL                
               GGGGGGGGGGGGGGGLLLLLLLLLLLLLLL               
               GGGGGGGGGGGGGGGLLLLLLLLLLLLLLL               
          GGGGGGGGGGGGGGGGGGGGLLLLLLLLLLLLLLLLLLLK          
    GGGGGGGGGGGGGGGGGGGGGGGGGGLLLLLLLLLLLLLLLLLLLLLLLLLL    
  GGGGGGGGGGGGGGGGGGGGGGGGGGGGLLLLLLLLLLLLLLLLLLLLLLLLLLLL  
  GGGGGGGGGGGGGGGGGGGGGGGGGGGGLLLLLLLLLLLLLLLLLLLLLLLLLLLL  
     GGGGGGGGGGGGGGGGGGGGGGGGGLLLLLLLLLLLLLLLLLLLLLLLLL     
            GGGGGGGGGGGGGGGGGGLLLLLLLLLLLLLLLLLK            
                                                            
                GGGGGGGGGGF     IIJKKKLLLLLL                
                GGGGGGGGGGGGGGLLLLLLLLLLLLLL                
                GGGGGGGGGGGGGGLLLLLLLLLLLLLK                
                 GGGGGGGGGGGGGLLLLLLLLLLLLL                 
                 GGGGGGGGGGGGGLLLLLLLLLLLLL                 
                 GGGGGGGGGGGGGLLLLLLLLLLLLL                 
                  GGGGGGGGGGGGLLLLLLLLLLLK                  
                    GGGGGGGGGGLLLLLLLLLL                    
                     GGGGGGGGGLLLLLLLLK                     
                       GGGGGGGLLLLLLLJ                      
                        GGGGGGLLLLLL                        
                           GGGLLL     
                                                            
  https://github.com/W0rthlessS0ul/nRF24_jammer                      
)rawliteral";

#define COL_WIDTH 22

void printCmd(const char* cmd, const char* desc, String& response, bool web) {
  String line = "  ";
  line += cmd;
  for (int i = strlen(cmd); i < COL_WIDTH; i++) line += ' ';
  line += " ";
  line += desc;
  Serial.println(line);
  if (web) response += line + "\n";
}

void printLine(const String& text, String& response, bool web) {
  Serial.println("  " + text);
  if (web) response += "  " + text + "\n";
}

void print(const String& text, String& response, bool web) {
  Serial.print(text);
  if (web) response += text;
}

bool checkModules(String& response, bool web) {
  if (nrf24_count <= 0) {
    String msg = "No nRF24 modules configured. Use 'set nrf24' or web interface";
    printLine(msg, response, web);
    return false;
  }
  return true;
}

bool CommandsHandler(String command, bool web_command) {
  String response;
  command.trim();
  if (command.length() == 0) return true;

  String args[10];
  int argCount = 0;
  int pos = 0;
  while (pos < command.length() && argCount < 10) {
    int space = command.indexOf(' ', pos);
    if (space == -1) {
      args[argCount++] = command.substring(pos);
      break;
    }
    args[argCount++] = command.substring(pos, space);
    pos = space + 1;
  }

  args[0].toLowerCase();

  if (args[0] == "help" || args[0] == "?") {
    printLine("", response, web_command);
    printCmd("jam", "Start jamming", response, web_command);
    printCmd("stop", "Stop current attack", response, web_command);
    printCmd("scan", "Start scanning", response, web_command);
    printCmd("set", "Configure device", response, web_command);
    printCmd("info", "System information", response, web_command);
    printCmd("reboot", "Restart device", response, web_command);
    printCmd("help", "Show this help", response, web_command);
    printLine("", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return true;
  }

  if (args[0] == "stop") {
    printLine("Stopping all attacks...", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return false;
  }

  if (args[0] == "reboot") {
    printLine("Rebooting...", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    delay(100);
    ESP.restart();
    return true;
  }

  if (args[0] == "info") {
    printLine("", response, web_command);
    printLine("Version          " + String(Version_Number) + " (" + Version_Name + ")", response, web_command);
    printLine("Display          " + String(SCREEN_WIDTH) + "x" + String(SCREEN_HEIGHT), response, web_command);
    printLine("Chip             " + String(ESP.getChipModel()) + " rev" + String(ESP.getChipRevision()), response, web_command);
    printLine("CPU Freq         " + String(ESP.getCpuFreqMHz()) + " MHz", response, web_command);
    printLine("Flash size       " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB", response, web_command);
    printLine("Free heap        " + String(ESP.getFreeHeap()) + " bytes", response, web_command);
    printLine("Uptime           " + String(millis() / 1000) + " s", response, web_command);
    printLine("nRF24 modules    " + String(nrf24_count), response, web_command);
    for (int i = 0; i < nrf24_count; i++) {
      printLine("  Module " + String(i) + ": CE=" + String(ce_pins[i]) + ", CSN=" + String(csn_pins[i]), response, web_command);
    }
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return true;
  }

  if (args[0] == "jam") {
    bool ret = handleJamCommand(args, argCount, web_command, response);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return ret;
  }

  if (args[0] == "scan") {
    bool ret = handleScanCommand(args, argCount, web_command, response);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return ret;
  }

  if (args[0] == "set") {
    bool ret = handleSetCommand(args, argCount, web_command, response);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return ret;
  }

  printLine("Unknown command Type 'help'", response, web_command);
  if (web_command) server.send(200, "text/html", response + "ok\n");
  return true;
}

bool handleJamCommand(String* args, int argCount, bool web_command, String& response) {
  if (argCount == 1) {
    printLine("", response, web_command);
    printCmd("bt", "Bluetooth jammer", response, web_command);
    printCmd("drone", "Drone jammer", response, web_command);
    printCmd("wifi", "WiFi jammer", response, web_command);
    printCmd("ble", "BLE jammer", response, web_command);
    printCmd("zigbee", "Zigbee jammer", response, web_command);
    printCmd("misc", "Custom channel range", response, web_command);
    printLine("", response, web_command);
    return true;
  }

  if (!checkModules(response, web_command)) {
    return true;
  }

  String sub = args[1];
  sub.toLowerCase();

  if (sub == "bt" || sub == "bluetooth") {
    printLine("Starting Bluetooth jammer...", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    jamHandler("Bluetooth Jamming", bluetooth_jam, bitmap_bluetooth_jam);
    return true;
  }

  if (sub == "drone") {
    printLine("Starting Drone jammer...", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    jamHandler("Drone Jamming", drone_jam, bitmap_drone_jam);
    return true;
  }

  if (sub == "wifi") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printCmd("all", "Interfere with all WiFi frequencies simultaneously", response, web_command);
      printCmd("smart", "Scan for active networks and jam only their", response, web_command);
      printCmd("channel <1-14>", "Target specific WiFi channel", response, web_command);
      printLine("", response, web_command);
      return true;
    }

    String wifiSub = args[2];
    wifiSub.toLowerCase();

    if (wifiSub == "all") {
      printLine("Starting WiFi all channels jammer...", response, web_command);
      if (web_command) server.send(200, "text/html", response + "ok\n");
      jamHandler("WiFi Jamming", wifi_jam, bitmap_wifi_jam);
      return true;
    }

    if (wifiSub == "smart") {
      printLine("Starting WiFi smart jammer...", response, web_command);
      if (web_command) server.send(200, "text/html", response + "ok\n");
      jamHandler("WiFi Jamming", wifi_scan_jam, bitmap_wifi_jam);
      return true;
    }

    if (wifiSub == "channel") {
      if (argCount < 4) {
        printLine("Missing channel number Usage: jam wifi channel <1-14>", response, web_command);
        return true;
      }
      int ch = args[3].toInt();
      if (ch < 1 || ch > 14) {
        printLine("Channel must be 1-14", response, web_command);
        return true;
      }
      printLine("Starting WiFi jammer on channel " + String(ch) + "...", response, web_command);
      if (web_command) server.send(200, "text/html", response + "ok\n");
      display.clearDisplay();
      display.drawBitmap(0, 0, bitmap_wifi_jam, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
      display.display();
      wifi_channel(ch);
      updateDisplay(menu_number);
      return true;
    }

    printLine("Unknown wifi option Type 'jam wifi' for list", response, web_command);
    return true;
  }

  if (sub == "ble") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printCmd("ad", "Interfere with the three primary BLE advertising channels", response, web_command);
      printCmd("data", "Interfere with all BLE data transmission channels", response, web_command);
      printLine("", response, web_command);
      return true;
    }

    String bleSub = args[2];
    bleSub.toLowerCase();

    if (bleSub == "ad" || bleSub == "advertising") {
      printLine("Starting BLE advertising jammer...", response, web_command);
      if (web_command) server.send(200, "text/html", response + "ok\n");
      jamHandler("BLE Jamming", ble_advertising_jam, bitmap_ble_jam);
      return true;
    }

    if (bleSub == "data") {
      printLine("Starting BLE data jammer...", response, web_command);
      if (web_command) server.send(200, "text/html", response + "ok\n");
      jamHandler("BLE Jamming", ble_data_jam, bitmap_ble_jam);
      return true;
    }

    printLine("Unknown BLE mode Type 'jam ble' for list", response, web_command);
    return true;
  }

  if (sub == "zigbee") {
    printLine("Starting Zigbee jammer...", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    jamHandler("Zigbee Jamming", zigbee_jam, bitmap_zigbee_jam);
    return true;
  }

  if (sub == "misc") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  jam misc <start> <stop>", response, web_command);
      printLine("    start : 0-125", response, web_command);
      printLine("    stop  : 0-125", response, web_command);
      printLine("", response, web_command);
      return true;
    }

    if (argCount < 4) {
      printLine("Missing start or stop channel", response, web_command);
      return true;
    }

    int start = args[2].toInt();
    int stop = args[3].toInt();
    if (start < 0 || start > 125 || stop < 0 || stop > 125 || stop < start) {
      printLine("Invalid range (0-125, stop >= start)", response, web_command);
      return true;
    }

    printLine("Starting misc jammer on channels " + String(start) + "-" + String(stop) + "...", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");

    display.clearDisplay();
    display.setCursor(0, 0); display.println("Start");
    display.setCursor(96, 0); display.println("Stop");
    display.setCursor(52, 0); display.println("Width");
    display.setCursor(8, 10); display.println(String(start));
    display.setCursor(60, 10); display.println(String(stop - start));
    display.setCursor(100, 10); display.println(String(stop));
    display.setCursor(0, 20); display.println("Jamming Started");
    display.display();

    misc_jam(start, stop);
    updateDisplay(menu_number);
    return true;
  }

  printLine("Unknown jam mode Type 'jam' for list", response, web_command);
  return true;
}

bool handleScanCommand(String* args, int argCount, bool web_command, String& response) {
  if (argCount == 1) {
    printLine("", response, web_command);
    printCmd("wifi", "Scan Wi-Fi SSIDs, channels", response, web_command);
    printLine("", response, web_command);
    return true;
  }

  String sub = args[1];
  sub.toLowerCase();

  if (sub == "wifi") {
    printLine("", response, web_command);
    printLine("Scanning Wi-Fi APs", response, web_command);
    int networks = scan_wifi_APs(WiFiScanChannels, false);
    printLine("Found " + String(networks) + " APs", response, web_command);
    printLine("", response, web_command);

    printLine("+----------------------------------+-------+", response, web_command);
    printLine("|               SSID               |Channel|", response, web_command);
    printLine("+----------------------------------+-------+", response, web_command);

    for (int channel = 1; channel <= 14; channel++) {
      int count = WiFiScanChannels[channel - 1];
      if (count == 0) continue;

      String allSsids = APs_array[channel - 1];
      int start = 0;
      for (int i = 0; i < count; i++) {
        int end = allSsids.indexOf('\n', start);
        String ssid = (end == -1) ? allSsids.substring(start) : allSsids.substring(start, end);
        start = end + 1;

        String line = "";
        int ssidLen = ssid.length();
        line += "|";
        for (int s = 0; s < 17 - ssidLen / 2; s++) line += " ";
        line += ssid;
        for (int s = 0; s < 17 - ssidLen / 2; s++) line += " ";
        if (ssidLen % 2 != 0) line.remove(line.length() - 1);
        line += "|";

        String chStr = String(channel);
        int chLen = chStr.length();
        for (int s = 0; s < 4 - chLen / 2; s++) line += " ";
        line += chStr;
        for (int s = 0; s < 7 - (4 - chLen / 2) - chLen; s++) line += " ";
        line += "|";

        printLine(line, response, web_command);
      }
    }

    printLine("+----------------------------------+-------+", response, web_command);
    if (web_command) server.send(200, "text/html", response + "ok\n");
    return true;
  }

  printLine("Unknown scan mode. Type 'scan' for list", response, web_command);
  return true;
}

bool handleSetCommand(String* args, int argCount, bool web_command, String& response) {
  if (argCount == 1) {
    printLine("", response, web_command);
    printCmd("bt", "Bluetooth settings", response, web_command);
    printCmd("drone", "Drone settings", response, web_command);
    printCmd("misc", "Misc settings", response, web_command);
    printCmd("logo", "Logo settings", response, web_command);
    printCmd("access_point", "Access point settings", response, web_command);
    printCmd("buttons", "Button configuration", response, web_command);
    printCmd("display", "Display settings", response, web_command);
    printCmd("sweep", "Channel sweep direction", response, web_command);
    printCmd("PA", "nRF24 PA settings", response, web_command);
    printCmd("nrf24", "nRF24 pins configure", response, web_command);
    printLine("", response, web_command);
    return true;
  }

  String param = args[1];
  param.toLowerCase();

  if (param == "bt" || param == "bluetooth_method") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Brute force by list (21 channels)", response, web_command);
      printLine("  1 - Randomize values (0-79)", response, web_command);
      printLine("  2 - Brute force by values (0-79)", response, web_command);
      printLine("Current: " + String(bluetooth_jam_method), response, web_command);
      printLine("Usage: set bt <0-2>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 2) {
      printLine("Value must be 0-2", response, web_command);
      return true;
    }
    storeEEPROMAndSet(0, val, bluetooth_jam_method);
    printLine("Bluetooth method set to " + String(val), response, web_command);
    return true;
  }

  if (param == "drone") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Randomize values (0-125)", response, web_command);
      printLine("  1 - Brute force by values (0-125)", response, web_command);
      printLine("Current: " + String(drone_jam_method), response, web_command);
      printLine("Usage: set drone_method <0-1>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 1) {
      printLine("Value must be 0-1", response, web_command);
      return true;
    }
    storeEEPROMAndSet(1, val, drone_jam_method);
    printLine("Drone method set to " + String(val), response, web_command);
    return true;
  }

  if (param == "misc") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Channel switching (fast)", response, web_command);
      printLine("  1 - Packet sending (slow)", response, web_command);
      printLine("Current: " + String(misc_jam_method), response, web_command);
      printLine("Usage: set misc_method <0-1>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 1) {
      printLine("Value must be 0-1", response, web_command);
      return true;
    }
    storeEEPROMAndSet(6, val, misc_jam_method);
    printLine("Misc method set to " + String(val), response, web_command);
    return true;
  }

  if (param == "logo") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Show logo", response, web_command);
      printLine("  1 - Hide logo", response, web_command);
      printLine("Current: " + String(logo), response, web_command);
      printLine("Usage: set logo <0-1>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 1) {
      printLine("Value must be 0-1", response, web_command);
      return true;
    }
    storeEEPROMAndSet(7, val, logo);
    printLine("Logo setting set to " + String(val), response, web_command);
    return true;
  }

  if (param == "access_point") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Enabled", response, web_command);
      printLine("  1 - Disabled", response, web_command);
      printLine("Current: " + String(access_point), response, web_command);
      printLine("Usage: set access_point <0-1>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 1) {
      printLine("Value must be 0-1", response, web_command);
      return true;
    }
    if (val == 1) {
      access_poin_off();
    } else {
      storeEEPROMAndSet(8, 0, access_point);
      printLine("Access point will be enabled after reboot", response, web_command);
      if (web_command) server.send(200, "text/html", response + "ok\n");
      ESP.restart();
    }
    return true;
  }

  if (param == "buttons") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Single-button (pin 25)", response, web_command);
      printLine("  1 - Two-button (pins 25,26)", response, web_command);
      printLine("  2 - Three-button (pins 25,26,27)", response, web_command);
      printLine("Current: " + String(buttons), response, web_command);
      printLine("Usage: set buttons <0-2>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 2) {
      printLine("Value must be 0-2", response, web_command);
      return true;
    }
    storeEEPROMAndSet(9, val, buttons);
    printLine("Button mode set to " + String(val), response, web_command);
    return true;
  }

  if (param == "display") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Enabled", response, web_command);
      printLine("  1 - Disabled", response, web_command);
      printLine("Current: " + String(display_setting), response, web_command);
      printLine("Usage: set display <0-1>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 1) {
      printLine("Value must be 0-1", response, web_command);
      return true;
    }
    storeEEPROMAndReset(2, val, display_setting);
    return true;
  }

  if (param == "sweep") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - Opposite directions", response, web_command);
      printLine("  1 - Same direction", response, web_command);
      printLine("Current: " + String(Separate_or_together), response, web_command);
      printLine("Usage: set sweep <0-1>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 1) {
      printLine("Value must be 0-1", response, web_command);
      return true;
    }
    storeEEPROMAndSet(4, val, Separate_or_together);
    printLine("Sweep direction set to " + String(val), response, web_command);
    return true;
  }

  if (param == "pa") {
    if (argCount == 2) {
      printLine("", response, web_command);
      printLine("  0 - MAX (0 dBm)", response, web_command);
      printLine("  1 - HIGH (-6 dBm)", response, web_command);
      printLine("  2 - LOW (-12 dBm)", response, web_command);
      printLine("  3 - MIN (-18 dBm)", response, web_command);
      printLine("Current: " + String(nrf_pa), response, web_command);
      printLine("Usage: set pa <0-3>", response, web_command);
      printLine("", response, web_command);
      return true;
    }
    int val = args[2].toInt();
    if (val < 0 || val > 3) {
      printLine("Value must be 0-3", response, web_command);
      return true;
    }
    storeEEPROMAndSet(5, val, nrf_pa);
    printLine("PA level set to " + String(val), response, web_command);
    return true;
  }

  if (param == "nrf24") {
    return handleSetNrf24Command(args, argCount, web_command, response);
  }

  printLine("Unknown setting Type 'set' for list", response, web_command);
  return true;
}

bool handleSetNrf24Command(String* args, int argCount, bool web_command, String& response) {
  if (argCount == 2) {
    printLine("", response, web_command);
    printLine("  Module count: " + String(nrf24_count), response, web_command);
    for (int i = 0; i < nrf24_count; i++) {
      printLine("    Module " + String(i) + ": CE=" + String(ce_pins[i]) + ", CSN=" + String(csn_pins[i]), response, web_command);
    }
    printLine("Usage: set nrf24 <ce1,csn1> [<ce2,csn2> ...]", response, web_command);
    printLine("Example: set nrf24 9,10 5,6", response, web_command);
    printLine("", response, web_command);
    return true;
  }

  String pinPairs[30];
  int pairCount = 0;
  for (int i = 2; i < argCount && pairCount < 30; i++) {
    pinPairs[pairCount++] = args[i];
  }

  int new_ce[30], new_csn[30];
  bool valid = true;
  for (int i = 0; i < pairCount; i++) {
    int comma = pinPairs[i].indexOf(',');
    if (comma == -1) {
      printLine("Invalid format for module " + String(i) + ". Expected CE,CSN", response, web_command);
      valid = false;
      break;
    }
    String ceStr = pinPairs[i].substring(0, comma);
    String csnStr = pinPairs[i].substring(comma + 1);
    new_ce[i] = ceStr.toInt();
    new_csn[i] = csnStr.toInt();
    if (new_ce[i] < 0 || new_ce[i] > 99 || new_csn[i] < 0 || new_csn[i] > 99) {
      printLine("Pin values for module " + String(i) + " must be 0-99", response, web_command);
      valid = false;
      break;
    }
  }

  if (!valid) {
    return true;
  }

  nrf24_count = pairCount;
  for (int i = 0; i < nrf24_count; i++) {
    ce_pins[i] = new_ce[i];
    csn_pins[i] = new_csn[i];
    EEPROM.write(74 + i, ce_pins[i]);
    EEPROM.write(104 + i, csn_pins[i]);
  }
  EEPROM.write(134, nrf24_count);
  EEPROM.commit();

  printLine("nRF24 configuration updated. Rebooting...", response, web_command);
  if (web_command) server.send(200, "text/html", response + "ok\n");
  delay(100);
  ESP.restart();
  return true;
}

bool SerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() == 0) return true;

    Serial.print("$ ");
    Serial.println(command);

    return CommandsHandler(command, false);
  }
  return true;
}