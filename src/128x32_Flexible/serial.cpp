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

bool CommandsHandler(String command, bool web_command){
  int FirstSpaceIndex = command.indexOf(' ');
  int SecondSpaceIndex = -1;
  String FirstWord = command;
  String SecondWord = "-1";
  String ThirdWord = "-1";

  if (FirstSpaceIndex != -1) {
    SecondSpaceIndex = command.indexOf(' ', FirstSpaceIndex + 1);
    FirstWord = command.substring(0, FirstSpaceIndex);
    if (SecondSpaceIndex != -1) {
      SecondWord = command.substring(FirstSpaceIndex + 1, SecondSpaceIndex);
      ThirdWord = command.substring(SecondSpaceIndex + 1);
    } else {
      SecondWord = command.substring(FirstSpaceIndex + 1);
    }
    SecondWord.trim();
    ThirdWord.trim();
  }

  bool shouldContinue = true;
  if (FirstWord == "help") {
    String responce = 
        "jam <mode> ==> Jamming in the selected mode; modes: bluetooth, drone, WiFi, BLE, zigbee\n"
        "miscjam <start channel> <end channel> ==> Jamming in the selected channel range; start/end channel: 0-125\n"
        "stop ==> Stop all attacks\n"
        "setting ==> Changing settings; write 'setting' to see more info\n"
        "reboot ==> Rebooting the device\n"
        "info ==> Output of information about the system";
    Serial.println(responce);
    Serial.println("ok");
    if (web_command){
      server.send(200, "text/html", responce+"\nok");
    }
  } else if (FirstWord == "stop") {
    String responce = "Stopping all attacks...";
    Serial.println(responce);
    Serial.println("ok");
    if (web_command){
      server.send(200, "text/html", responce+"\nok");
    }
    shouldContinue = false;
  } else if (FirstWord == "miscjam") {
    if (SecondWord == "-1" || ThirdWord == "-1") {
      String responce = "No start/end channel specified\nmiscjam <start channel> <end channel> ==> Jamming in the selected channel range; start/end channel: 0-125";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (nrf24_count <= 0) {
      String responce = "nRF24 not configured \nconfigure using 'setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]' OR using Web Interface";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord.toInt() > ThirdWord.toInt()) {
      String responce = "end channel must be larger than start one";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord.toInt() < 0 || SecondWord.toInt() > 125 ||
                ThirdWord.toInt() < 0 || ThirdWord.toInt() > 125) {
      String responce = "start/end channel must be in range 0-125";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else {
      String responce = "Jamming from " + String(SecondWord) + " to " + String(ThirdWord) + "...";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
      String html = FPSTR(html_jam);

      String jamRange = "Jamming from " + String(SecondWord) + " to " + String(ThirdWord);
      html.replace("[||]EdItAbLe TeXt[||]", jamRange);
      sendHtmlAndExecute(html.c_str(), nullptr); 
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Start");
      display.setCursor(96, 0);
      display.println("Stop");
      display.setCursor(52, 0);
      display.println("Width");
      display.setCursor(8, 10);
      display.println(SecondWord);
      display.setCursor(60, 10);
      display.println(String(ThirdWord.toInt() - SecondWord.toInt()));
      display.setCursor(100, 10);
      display.println(ThirdWord);
      display.setCursor(0, 20);
      display.println("Jamming Started");
      display.display();
      
      misc_jam(SecondWord.toInt(), ThirdWord.toInt());
      
      html.replace(jamRange, "[||]EdItAbLe TeXt[||]");
      updateDisplay(menu_number);
    }
  } else if (FirstWord == "jam") {
    if (SecondWord == "-1") {
      String responce = "No jamming mode specified\njam <mode> ==> Available modes: bluetooth, drone, WiFi, BLE, zigbee";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (nrf24_count <= 0) {
      String responce = "nRF24 not configured \nconfigure using 'setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]' OR using Web Interface";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "bluetooth") {
      String responce = "Jamming bluetooth...";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }

      jamHandler(String("Bluetooth Jamming"), bluetooth_jam, bitmap_bluetooth_jam);
    } else if (SecondWord == "drone") {
      String responce = "Jamming drone...";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }

      jamHandler(String("Drone Jamming"), drone_jam, bitmap_drone_jam);
    } else if (SecondWord == "wifi") {
      if (ThirdWord == "all") {
        String responce = "Jamming all WiFi channels...";
        Serial.println(responce);
        Serial.println("ok");
        if (web_command){
          server.send(200, "text/html", responce+"\nok");
        }

        jamHandler(String("WiFi Jamming"), wifi_jam, bitmap_wifi_jam);
      } else if (ThirdWord == "smart") {
        String responce = "Smart Jamming WiFi channels...";
        Serial.println(responce);
        Serial.println("ok");
        if (web_command){
          server.send(200, "text/html", responce+"\nok");
        }

        jamHandler(String("WiFi Jamming"), wifi_scan_jam, bitmap_wifi_jam);
      } else if (ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 13) {
        String responce = "Jamming " + String(ThirdWord) + " WiFi channel...";
        Serial.println(responce);
        Serial.println("ok");
        if (web_command){
          server.send(200, "text/html", responce+"\nok");
        }
        String html = FPSTR(html_jam);
        
        String wifiChannelMsg = "Jamming " + String(ThirdWord) + " WiFi Channel";
        html.replace("[||]EdItAbLe TeXt[||]", wifiChannelMsg);
        display.clearDisplay();
        display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 64, WHITE);
        display.display();
        sendHtmlAndExecute(html.c_str(), nullptr);
        wifi_channel(ThirdWord.toInt());
        
        html.replace(wifiChannelMsg, "[||]EdItAbLe TeXt[||]");
        updateDisplay(menu_number);
      } else {
        String responce = "No WiFi channel specified\njam WiFi <channel> ==> WiFi channel must be in range 0-13 OR 'all' for all channels OR 'smart' for auto chosing channels";
        Serial.println(responce);
        Serial.println("ok");
        if (web_command){
          server.send(200, "text/html", responce+"\nok");
        }
      }
    } else if (SecondWord == "ble") {
      String responce = "Jamming BLE...";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }

      jamHandler(String("BLE Jamming"), ble_jam, bitmap_ble_jam);
    } else if (SecondWord == "zigbee") {
      String responce = "Jamming zigbee...";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
      
      jamHandler(String("Zigbee Jamming"), zigbee_jam, bitmap_zigbee_jam);
    } else {
      String responce = "Unknown parameters";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    }
  } else if (FirstWord == "setting") {
    if (SecondWord == "-1") {
      struct SettingInfo {
        const char *name;
        int currentValue;
        const char *options;
      };

      SettingInfo settings[] = {
          {"BluetoothJamMethod", bluetooth_jam_method,
            "0 - Brute Force By List (List 21), 1 - Randomize Values (0-79), "
            "2 - Brute Force By Values (0-79)"},
          {"DroneJamMethod", drone_jam_method,
            "0 - Randomize Values (0-125), 1 - Brute Force By Values (0-125)"},
          {"MiscJamMethod", misc_jam_method,
            "0 - Channel Switching (fast mode; used in Bluetooth Jam), 1 - "
            "Packet Sending (slow mode; used in WiFi Jam)"},
          {"Logo", logo,
            "0 - Show logo on startup, 1 - Hide logo on startup"},
          {"AccessPoint", access_point,
            "0 - Enable access point, 1 - Disable access point"},
          {"Buttons", buttons,
            "0 - Single-button control (25 pin), 1 - Two-button control (25, "
            "26 pin), 2 - Three-button control (25, 26, 27 pin)"},
          {"Display", display_setting,
            "0 - Enable display, 1 - Disable display"},
          {"JammingType", Separate_or_together,
            "0 - Modules on Different Channels, 1 - Modules on Same Channel"},
          {"PA", nrf_pa,
            "0 - MAX (0dBm), 1 - HIGH (-6dBm), 2 - LOW (-12dBm), 3 - MIN (-18dBm)"}};

      String responce = "setting <setting name> <value>\n";
      for (auto &setting : settings) {
        String options = String(setting.options);
        options.replace(String(setting.currentValue) + " -",
                        String(setting.currentValue) + "* -");
        
        responce += "  name: " + String(setting.name) + "\n   values: " + options + "\n";
      }
      
      responce += "  name: nRF24\n    current configuration:\n      Module count: " + String(nrf24_count) + "\n";

      for (int i = 0; i < nrf24_count; i++) {
        responce += "      Module " + String(i) + ": CE=" +
                        String(ce_pins[i]) + ", CSN=" + String(csn_pins[i]) + "\n";
      }
      responce += "    usage: setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]\n";
      
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "bluetoothjammethod" && ThirdWord.toInt() >= 0 &&
                ThirdWord.toInt() <= 2) {
      storeEEPROMAndSet(0, ThirdWord.toInt(), bluetooth_jam_method);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "dronejammethod" && ThirdWord.toInt() >= 0 &&
                ThirdWord.toInt() <= 1) {
      storeEEPROMAndSet(1, ThirdWord.toInt(), drone_jam_method);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "miscjammethod" && ThirdWord.toInt() >= 0 &&
                ThirdWord.toInt() <= 1) {
      storeEEPROMAndSet(6, ThirdWord.toInt(), misc_jam_method);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "logo" && ThirdWord.toInt() >= 0 &&
                ThirdWord.toInt() <= 1) {
      storeEEPROMAndSet(7, ThirdWord.toInt(), logo);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "accesspoint" && ThirdWord.toInt() >= 0 &&
                ThirdWord.toInt() <= 1) {
      String responce = "Device will be rebooted";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
      if (ThirdWord.toInt() == 1) {
        access_poin_off();
      } else {
        storeEEPROMAndSet(8, ThirdWord.toInt(), access_point);
      }
      ESP.restart();
    } else if (SecondWord == "buttons" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 2) {
      storeEEPROMAndSet(9, ThirdWord.toInt(), buttons);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "display" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 2) {
      String responce = "Device will be rebooted";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
      storeEEPROMAndReset(2, ThirdWord.toInt(), display_setting);
    } else if (SecondWord == "jammingtype" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 1) {
      storeEEPROMAndSet(4, ThirdWord.toInt(), Separate_or_together);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "pa" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 3) {
      storeEEPROMAndSet(5, ThirdWord.toInt(), nrf_pa);
      String responce = "Written successfully";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    } else if (SecondWord == "nrf24") {
      if (ThirdWord == "-1") {
        String responce = "Current nRF24 configuration:\n  Module count: "+String(nrf24_count)+"\n";
        for (int i = 0; i < nrf24_count; i++) {
          responce += "  Module "+String(i)+": CE="+String(ce_pins[i])+", CSN="+String(csn_pins[i])+"\n";
        }
        responce += "To configure nRF24 modules use:\n  setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]";
        Serial.println(responce);
        Serial.println("ok");
        if (web_command){
          server.send(200, "text/html", responce+"\nok");
        }
      } else {
        String args = command.substring(command.indexOf("nrf24") + 5);
        args.trim();
        String pinPairs[30];
        int pairCount = 0;
        int start = 0;
        while (start < args.length() && pairCount < 30) {
          int end = args.indexOf(' ', start);
          if (end == -1)
            end = args.length();
          String pair = args.substring(start, end);
          if (pair.length() > 0) {
            pinPairs[pairCount] = pair;
            pairCount++;
          }
          start = end + 1;
        }

        if (pairCount <= 0 || pairCount > 30) {
          String responce = "Module count must be between 1 and 30";
          Serial.println(responce);
          Serial.println("ok");
          if (web_command){
            server.send(200, "text/html", responce+"\nok");
          }
        } else {
          int new_ce_pins[30];
          int new_csn_pins[30];
          bool valid = true;
          for (int i = 0; i < pairCount; i++) {
            int commaIndex = pinPairs[i].indexOf(',');
            if (commaIndex == -1) {
              String responce = "Invalid pin pair format for module " + String(i);
              responce += "\nExpected format: CE,CSN";
              Serial.println(responce);
              if (web_command){
                server.send(200, "text/html", responce+"\nok");
              }
              valid = false;
              break;
            }
            String ceStr = pinPairs[i].substring(0, commaIndex);
            String csnStr = pinPairs[i].substring(commaIndex + 1);
            new_ce_pins[i] = ceStr.toInt();
            new_csn_pins[i] = csnStr.toInt();
            if (new_ce_pins[i] < 0 || new_ce_pins[i] > 99 ||
                new_csn_pins[i] < 0 || new_csn_pins[i] > 99) {
              String responce = "Pin values must be between 0 and 99 for module " + String(i);
              Serial.println(responce);
              if (web_command){
                server.send(200, "text/html", responce+"\nok");
              }
              valid = false;
              break;
            }
          }
          if (valid) {
            nrf24_count = pairCount;
            for (int i = 0; i < nrf24_count; i++) {
              ce_pins[i] = new_ce_pins[i];
              csn_pins[i] = new_csn_pins[i];
            }
            for (int i = 0; i < nrf24_count; i++) {
              EEPROM.write(74 + i, ce_pins[i]);
              EEPROM.write(104 + i, csn_pins[i]);
            }
            EEPROM.write(134, nrf24_count);
            EEPROM.commit();
            String responce = "nRF24 configuration updated successfully:\n";
            responce += "  Module count: " + String(nrf24_count) + "\n";
            for (int i = 0; i < nrf24_count; i++) {
              responce += "  Module "+String(i)+": CE="+String(ce_pins[i])+", CSN="+String(csn_pins[i])+"\n";
            }
            responce += "Device will be rebooted\n";
            Serial.println(responce);
            Serial.println("ok");
            if (web_command){
              server.send(200, "text/html", responce+"\nok");
            }
            ESP.restart();
          } else {
            Serial.println("ok");
            if (web_command){
              server.send(200, "text/html", "ok");
            }
          }
        }
      }
    } else {
      String responce = "Unknown parameters";
      Serial.println(responce);
      Serial.println("ok");
      if (web_command){
        server.send(200, "text/html", responce+"\nok");
      }
    }
  } else if (FirstWord == "info") {
    String responce = "Version Number:    " + String(Version_Number) + "\n";
    responce += "Version Name:      " + String(Version_Name) + "\n";
    responce += "Display Size:      " + String(SCREEN_WIDTH) + "x" + String(SCREEN_HEIGHT) + "\n";

    responce += "Chip Model:        " + String(ESP.getChipModel()) + "\n";
    responce += "Chip Cores:        " + String(ESP.getChipCores()) + "\n";
    responce += "Chip Revision:     " + String(ESP.getChipRevision()) + "\n";
    responce += "CPU Frequency:     " + String(ESP.getCpuFreqMHz()) + " MHz\n";

    uint32_t flashSize = ESP.getFlashChipSize();
    uint32_t freeSketchSpace = ESP.getFreeSketchSpace();
    uint32_t usedFlash = flashSize - freeSketchSpace;
    float flashUsedPercent = (usedFlash * 100.0) / flashSize;

    responce += "Flash Chip Size:   " + String(flashSize / (1024 * 1024)) + " MB\n";
    responce += "Flash Used:        " + String(usedFlash / 1024) + " KB\n";
    responce += "Flash Free:        " + String(freeSketchSpace / 1024) + " KB\n";
    responce += "Flash Usage:       " + String(flashUsedPercent, 1) + "%\n";
    responce += "Flash Chip Speed:  " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz\n";

    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t heapSize = ESP.getHeapSize();
    uint32_t usedHeap = heapSize - freeHeap;
    float heapUsedPercent = (usedHeap * 100.0) / heapSize;

    responce += "Heap Size:         " + String(heapSize) + " bytes\n";
    responce += "Heap Used:         " + String(usedHeap) + " bytes\n";
    responce += "Free Heap:         " + String(freeHeap) + " bytes\n";
    responce += "Heap Usage:        " + String(heapUsedPercent, 1) + "%\n";

    if (ESP.getPsramSize() > 0) {
      uint32_t freePSRAM = ESP.getFreePsram();
      uint32_t psramSize = ESP.getPsramSize();
      uint32_t usedPSRAM = psramSize - freePSRAM;
      float psramUsedPercent = (usedPSRAM * 100.0) / psramSize;

      responce += "PSRAM Size:        " + String(psramSize) + " bytes\n";
      responce += "PSRAM Used:        " + String(usedPSRAM) + " bytes\n";
      responce += "Free PSRAM:        " + String(freePSRAM) + " bytes\n";
      responce += "PSRAM Usage:       " + String(psramUsedPercent, 1) + "%\n";
    } else {
      responce += "PSRAM:             Not available\n";
    }

    responce += "SDK Version:       " + String(ESP.getSdkVersion()) + "\n";
    responce += "Arduino Version:   " + String(ARDUINO) + "\n";
    responce += "WiFi MAC Address:  " + String(WiFi.macAddress()) + "\n";

    responce += "Uptime:            " + String(millis() / 1000) + " seconds\n";
    responce += "Min Free Heap:     " + String(ESP.getMinFreeHeap()) + " bytes\n";
    responce += "Max Alloc Heap:    " + String(ESP.getMaxAllocHeap()) + " bytes";
    Serial.println(responce);
    Serial.println("ok");
    if (web_command){
      server.send(200, "text/html", responce+"\nok");
    }
  } else if (FirstWord == "reboot") {
    Serial.println("ok");
    if (web_command){
      server.send(200, "text/html", "ok");
    }
    ESP.restart();
  } else {
    String responce = "Unknown command";
    Serial.println(responce);
    Serial.println("ok");
    if (web_command){
      server.send(200, "text/html", responce+"\nok");
    }
  }
  return shouldContinue;
}

bool SerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();

    return CommandsHandler(command, false);
  }
  return true;
}