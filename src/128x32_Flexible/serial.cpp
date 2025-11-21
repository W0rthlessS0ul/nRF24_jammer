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

bool SerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();

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
      Serial.println(
          "jam <mode> ==> Jamming in the selected mode; modes: bluetooth, drone, WiFi, BLE, zigbee\n"
          "miscjam <start channel> <end channel> ==> Jamming in the selected channel range; start/end channel: 0-125\n"
          "stop ==> Stop all attacks\n"
          "setting ==> Changing settings; write 'setting' to see more info\n"
          "reboot ==> Rebooting the device\n"
          "info ==> Output of information about the system");
      Serial.println("ok");
    } else if (FirstWord == "stop") {
      Serial.println("Stopping all attacks...");
      Serial.println("ok");
      shouldContinue = false;
    } else if (FirstWord == "miscjam") {
      if (SecondWord == "-1" || ThirdWord == "-1") {
        Serial.println("No start/end channel specified\nmiscjam <start channel> <end channel> ==> Jamming in the selected channel range; start/end channel: 0-125");
        Serial.println("ok");
      } else if (nrf24_count <= 0) {
        Serial.println("nRF24 not configured \nconfigure using 'setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]' OR using Web Interface");
        Serial.println("ok");
      } else if (SecondWord.toInt() > ThirdWord.toInt()) {
        Serial.println("end channel must be larger than start one");
        Serial.println("ok");
      } else if (SecondWord.toInt() < 0 || SecondWord.toInt() > 125 ||
                 ThirdWord.toInt() < 0 || ThirdWord.toInt() > 125) {
        Serial.println("start/end channel must be in range 0-125");
        Serial.println("ok");
      } else {
        Serial.println("Jamming from " + String(SecondWord) + " to " + String(ThirdWord) + "...");
        Serial.println("ok");
        html_jam.replace("[||]EdItAbLe TeXt[||]", "Jamming from "+String(channel1)+" to "+String(channel2));
        sendHtmlAndExecute(html_jam.c_str(), nullptr); 
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
        html_jam.replace("Jamming from "+String(channel1)+" to "+String(channel2), "[||]EdItAbLe TeXt[||]");
        updateDisplay(menu_number);
      }
    } else if (FirstWord == "jam") {
      if (SecondWord == "-1") {
        Serial.println("No jamming mode specified\njam <mode> ==> Available modes: bluetooth, drone, WiFi, BLE, zigbee");
        Serial.println("ok");
      } else if (nrf24_count <= 0) {
        Serial.println("nRF24 not configured \nconfigure using 'setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]' OR using Web Interface");
        Serial.println("ok");
      } else if (SecondWord == "bluetooth") {
        Serial.println("Jamming bluetooth...");
        Serial.println("ok");
        jamHandler(String("Bluetooth Jamming"), bluetooth_jam, bitmap_bluetooth_jam);
      } else if (SecondWord == "drone") {
        Serial.println("Jamming drone...");
        Serial.println("ok");
        jamHandler(String("Drone Jamming"), drone_jam, bitmap_drone_jam);
      } else if (SecondWord == "wifi") {
        if (ThirdWord == "all") {
          Serial.println("Jamming all WiFi channels...");
          Serial.println("ok");
          jamHandler(String("WiFi Jamming"), wifi_jam, bitmap_wifi_jam);
        } else if (ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 13) {
          Serial.println("Jamming " + String(ThirdWord) + " WiFi channel...");
          Serial.println("ok");
          display.clearDisplay();
          display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
          display.display();
          html_jam.replace("[||]EdItAbLe TeXt[||]", "Jamming "+ThirdWord+" WiFi Channel");
          sendHtmlAndExecute(html_jam.c_str(), nullptr);
          wifi_channel(ThirdWord.toInt());
          html_jam.replace("Jamming "+ThirdWord+" WiFi Channel", "[||]EdItAbLe TeXt[||]");
          updateDisplay(menu_number);
        } else {
          Serial.println("No WiFi channel specified\njam WiFi <channel> ==> WiFi channel must be in range 0-13 OR 'all' for all channels");
          Serial.println("ok");
        }
      } else if (SecondWord == "ble") {
        Serial.println("Jamming BLE...");
        Serial.println("ok");
        jamHandler(String("BLE Jamming"), ble_jam, bitmap_ble_jam);
      } else if (SecondWord == "zigbee") {
        Serial.println("Jamming zigbee...");
        Serial.println("ok");
        jamHandler(String("Zigbee Jamming"), zigbee_jam, bitmap_zigbee_jam);
      } else {
        Serial.println("Unknown parameters");
        Serial.println("ok");
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
             "0 - Modules on Different Channels, 1 - Modules on Same Channel"}};

        Serial.println("setting <setting name> <value>");
        for (auto &setting : settings) {
          Serial.print("  name: ");
          Serial.println(setting.name);
          Serial.print("   values: ");

          String options = String(setting.options);
          options.replace(String(setting.currentValue) + " -",
                          String(setting.currentValue) + "* -");

          Serial.println(options);
        }

        Serial.println("  name: nRF24");
        Serial.println("    current configuration:");
        Serial.println("      Module count: " + String(nrf24_count));
        for (int i = 0; i < nrf24_count; i++) {
          Serial.println("      Module " + String(i) + ": CE=" +
                         String(ce_pins[i]) + ", CSN=" + String(csn_pins[i]));
        }
        Serial.println("    usage: setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]");
        Serial.println("ok");
      } else if (SecondWord == "bluetoothjammethod" && ThirdWord.toInt() >= 0 &&
                 ThirdWord.toInt() <= 2) {
        storeEEPROMAndSet(0, ThirdWord.toInt(), bluetooth_jam_method);
        Serial.println("Written successfully");
        Serial.println("ok");
      } else if (SecondWord == "dronejammethod" && ThirdWord.toInt() >= 0 &&
                 ThirdWord.toInt() <= 1) {
        storeEEPROMAndSet(1, ThirdWord.toInt(), drone_jam_method);
        Serial.println("Written successfully");
        Serial.println("ok");
      } else if (SecondWord == "miscjammethod" && ThirdWord.toInt() >= 0 &&
                 ThirdWord.toInt() <= 1) {
        storeEEPROMAndSet(6, ThirdWord.toInt(), misc_jam_method);
        Serial.println("Written successfully");
        Serial.println("ok");
      } else if (SecondWord == "logo" && ThirdWord.toInt() >= 0 &&
                 ThirdWord.toInt() <= 1) {
        storeEEPROMAndSet(7, ThirdWord.toInt(), logo);
        Serial.println("Written successfully");
        Serial.println("ok");
      } else if (SecondWord == "accesspoint" && ThirdWord.toInt() >= 0 &&
                 ThirdWord.toInt() <= 1) {
        Serial.println("Device will be rebooted");
        Serial.println("ok");
        if (ThirdWord.toInt() == 1) {
          access_poin_off();
        } else {
          storeEEPROMAndSet(8, ThirdWord.toInt(), access_point);
        }
        ESP.restart();
      } else if (SecondWord == "buttons" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 2) {
        storeEEPROMAndSet(9, ThirdWord.toInt(), buttons);
        Serial.println("Written successfully");
        Serial.println("ok");
      } else if (SecondWord == "display" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 2) {
        Serial.println("Device will be rebooted");
        Serial.println("ok");
        storeEEPROMAndReset(2, ThirdWord.toInt(), display_setting);
      } else if (SecondWord == "jammingtype" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 1) {
        storeEEPROMAndSet(4, ThirdWord.toInt(), Separate_or_together);
        Serial.println("Written successfully");
        Serial.println("ok");
      } else if (SecondWord == "nrf24") {
        if (ThirdWord == "-1") {
          Serial.println("Current nRF24 configuration:");
          Serial.print("  Module count: ");
          Serial.println(nrf24_count);
          for (int i = 0; i < nrf24_count; i++) {
            Serial.print("  Module ");
            Serial.print(i);
            Serial.print(": CE=");
            Serial.print(ce_pins[i]);
            Serial.print(", CSN=");
            Serial.println(csn_pins[i]);
          }
          Serial.println("To configure nRF24 modules use:");
          Serial.println("  setting nRF24 <ce1>,<csn1> [<ce2>,<csn2> ...]");
          Serial.println("ok");
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
            Serial.println("Module count must be between 1 and 30");
            Serial.println("ok");
          } else {
            int new_ce_pins[30];
            int new_csn_pins[30];
            bool valid = true;
            for (int i = 0; i < pairCount; i++) {
              int commaIndex = pinPairs[i].indexOf(',');
              if (commaIndex == -1) {
                Serial.println("Invalid pin pair format for module " + String(i));
                Serial.println("Expected format: CE,CSN");
                valid = false;
                break;
              }
              String ceStr = pinPairs[i].substring(0, commaIndex);
              String csnStr = pinPairs[i].substring(commaIndex + 1);
              new_ce_pins[i] = ceStr.toInt();
              new_csn_pins[i] = csnStr.toInt();
              if (new_ce_pins[i] < 0 || new_ce_pins[i] > 99 ||
                  new_csn_pins[i] < 0 || new_csn_pins[i] > 99) {
                Serial.println("Pin values must be between 0 and 99 for module " + String(i));
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
              Serial.println("nRF24 configuration updated successfully:");
              Serial.print("  Module count: ");
              Serial.println(nrf24_count);
              for (int i = 0; i < nrf24_count; i++) {
                Serial.print("  Module ");
                Serial.print(i);
                Serial.print(": CE=");
                Serial.print(ce_pins[i]);
                Serial.print(", CSN=");
                Serial.println(csn_pins[i]);
              }
              Serial.println("Device will be rebooted");
              Serial.println("ok");
              ESP.restart();
            } else {
              Serial.println("ok");
            }
          }
        }
      } else {
        Serial.println("Unknown parameters");
        Serial.println("ok");
      }
    } else if (FirstWord == "info") {
      Serial.println("Version Number:    " + String(Version_Number));
      Serial.println("Version Name:      " + String(Version_Name));
      Serial.println("Display Size:      " + String(SCREEN_WIDTH) + "x" + String(SCREEN_HEIGHT));

      Serial.println("Chip Model:        " + String(ESP.getChipModel()));
      Serial.println("Chip Cores:        " + String(ESP.getChipCores()));
      Serial.println("Chip Revision:     " + String(ESP.getChipRevision()));
      Serial.println("CPU Frequency:     " + String(ESP.getCpuFreqMHz()) + " MHz");

      uint32_t flashSize = ESP.getFlashChipSize();
      uint32_t freeSketchSpace = ESP.getFreeSketchSpace();
      uint32_t usedFlash = flashSize - freeSketchSpace;
      float flashUsedPercent = (usedFlash * 100.0) / flashSize;

      Serial.println("Flash Chip Size:   " + String(flashSize / (1024 * 1024)) + " MB");
      Serial.println("Flash Used:        " + String(usedFlash / 1024) + " KB");
      Serial.println("Flash Free:        " + String(freeSketchSpace / 1024) + " KB");
      Serial.println("Flash Usage:       " + String(flashUsedPercent, 1) + "%");
      Serial.println("Flash Chip Speed:  " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz");

      uint32_t freeHeap = ESP.getFreeHeap();
      uint32_t heapSize = ESP.getHeapSize();
      uint32_t usedHeap = heapSize - freeHeap;
      float heapUsedPercent = (usedHeap * 100.0) / heapSize;

      Serial.println("Heap Size:         " + String(heapSize) + " bytes");
      Serial.println("Heap Used:         " + String(usedHeap) + " bytes");
      Serial.println("Free Heap:         " + String(freeHeap) + " bytes");
      Serial.println("Heap Usage:        " + String(heapUsedPercent, 1) + "%");

      if (ESP.getPsramSize() > 0) {
        uint32_t freePSRAM = ESP.getFreePsram();
        uint32_t psramSize = ESP.getPsramSize();
        uint32_t usedPSRAM = psramSize - freePSRAM;
        float psramUsedPercent = (usedPSRAM * 100.0) / psramSize;

        Serial.println("PSRAM Size:        " + String(psramSize) + " bytes");
        Serial.println("PSRAM Used:        " + String(usedPSRAM) + " bytes");
        Serial.println("Free PSRAM:        " + String(freePSRAM) + " bytes");
        Serial.println("PSRAM Usage:       " + String(psramUsedPercent, 1) + "%");
      } else {
        Serial.println("PSRAM:             Not available");
      }

      Serial.println("SDK Version:       " + String(ESP.getSdkVersion()));
      Serial.println("Arduino Version:   " + String(ARDUINO));
      Serial.println("WiFi MAC Address:  " + String(WiFi.macAddress()));

      Serial.println("Uptime:            " + String(millis() / 1000) + " seconds");
      Serial.println("Min Free Heap:     " + String(ESP.getMinFreeHeap()) + " bytes");
      Serial.println("Max Alloc Heap:    " + String(ESP.getMaxAllocHeap()) + " bytes");
      Serial.println("ok");
    } else if (FirstWord == "reboot") {
      Serial.println("ok");
      ESP.restart();
    } else {
      Serial.println("Unknown command");
      Serial.println("ok");
    }

    return shouldContinue;
  }
  return true;
}