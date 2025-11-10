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

    FirstWord.toLowerCase();
    SecondWord.toLowerCase();
    ThirdWord.toLowerCase();

    if (FirstWord == "help") {
      Serial.println("jam <mode> ==> Jamming in the selected mode; modes: bluetooth, drone, WiFi, BLE, zigbee\n"
                     "miscjam <start channel> <end channel> ==> Jamming in the selected channel range; start/end channel: 0-125\n"
                     "stop ==> Stop all attacks\n"
                     "setting ==> Changing settings; write 'setting' to see more info\n"
                     "reboot ==> Rebooting the device\n"
                     "info ==> Output of information about the system");
    } else if (FirstWord == "stop") {
      Serial.print("Stopping all attacks...\n");
      return false;
    } else if (FirstWord == "miscjam") {
      if (SecondWord == "-1" || ThirdWord == "-1") {
        Serial.println("No start/end channel specified\nmiscjam <start channel> <end channel> ==> Jamming in the selected channel range; start/end channel: 0-125\n");
      }else if (SecondWord.toInt() > ThirdWord.toInt()) {
        Serial.println("end channel must be larger than start one\n");
      } else if (SecondWord.toInt() < 0 || SecondWord.toInt() > 125 || ThirdWord.toInt() < 0 || ThirdWord.toInt() > 125) {
        Serial.println("start/end channel must be in range 0-125\n");
      } else {
        Serial.println("Jamming from " + String(SecondWord) + " to " + String(ThirdWord) + "...\n");
        misc_jam(SecondWord.toInt(), ThirdWord.toInt());
      }
    } else if (FirstWord == "jam") {
      if (SecondWord == "-1"){
        Serial.println("No jamming mode specified\njam <mode> ==> Available modes: bluetooth, drone, WiFi, BLE, zigbee\n");
      }else if (SecondWord == "bluetooth") {
        Serial.println("Jamming bluetooth...\n");
        jamHandler(html_bluetooth_jam, bluetooth_jam);
      } else if (SecondWord == "drone") {
        Serial.println("Jamming drone...\n");
        jamHandler(html_drone_jam, drone_jam);
      } else if (SecondWord == "wifi") {
        if (ThirdWord == "all"){
          Serial.println("Jamming all WiFi channels...\n");
          jamHandler(html_wifi_jam, wifi_jam);
        }
        else if (ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 13){
          Serial.println("Jamming " + String(ThirdWord) + " WiFi channel...\n");
          wifi_channel(ThirdWord.toInt());
        }
        else {
          Serial.println("No WiFi channel specified\njam WiFi <channel> ==> WiFi channel must be in range 0-13 OR 'all' for all channels\n");
        }
      } else if (SecondWord == "ble") {
        Serial.println("Jamming BLE...\n");
        jamHandler(html_ble_jam, ble_jam);
      } else if (SecondWord == "zigbee") {
        Serial.println("Jamming zigbee...\n");
        jamHandler(html_zigbee_jam, zigbee_jam);
      } else {
        Serial.println("Unknown parameters\n");
      }
    } else if (FirstWord == "setting"){
      if (SecondWord == "-1" || ThirdWord == "-1"){
        struct SettingInfo {
          const char* name;
          int currentValue;
          const char* options;
        };
        
        SettingInfo settings[] = {
          {"BluetoothJamMethod", bluetooth_jam_method, "0 - Brute Force By List (List 21), 1 - Randomize Values (0-79), 2 - Brute Force By Values (0-79)"},
          {"DroneJamMethod", drone_jam_method, "0 - Randomize Values (0-125), 1 - Brute Force By Values (0-125)"},
          {"MiscJamMethod", misc_jam_method, "0 - Channel Switching (fast mode; used in Bluetooth Jam), 1 - Packet Sending (slow mode; used in WiFi Jam)"},
          {"JammingType", Separate_or_together, "0 - Modules on Different Channels, 1 - Modules on Same Channel"},
        };
        
        Serial.println("setting <setting name> <value>");
        for (auto& setting : settings) {
          Serial.print("  name: ");
          Serial.print(setting.name);
          Serial.print(" values: ");

          String options = String(setting.options);
          options.replace(String(setting.currentValue) + " -", 
                        String(setting.currentValue) + "* -");
          
          Serial.println(options);
        }
        
      } else if (SecondWord == "bluetoothjammethod" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 2){
        storeEEPROMAndSet(0, ThirdWord.toInt(), bluetooth_jam_method);
        Serial.println("Written successfully\n");
      } else if (SecondWord == "dronejammethod" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 1){
        storeEEPROMAndSet(1, ThirdWord.toInt(), drone_jam_method);
        Serial.println("Written successfully\n");
      } else if (SecondWord == "miscjammethod" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 1){
        storeEEPROMAndSet(6, ThirdWord.toInt(), misc_jam_method);
        Serial.println("Written successfully\n");
      } else if (SecondWord == "jammingtype" && ThirdWord.toInt() >= 0 && ThirdWord.toInt() <= 1){
        storeEEPROMAndSet(4, ThirdWord.toInt(), Separate_or_together);
        Serial.println("Written successfully\n");
      } else {
        Serial.println("Unknown parameters\n");
      }
    } else if (FirstWord == "info"){
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

      if(ESP.getPsramSize() > 0){
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
    } else if (FirstWord == "reboot"){
      ESP.restart();
    } else {
      Serial.println("Unknown command\n");
    }
  }
  return true;
}