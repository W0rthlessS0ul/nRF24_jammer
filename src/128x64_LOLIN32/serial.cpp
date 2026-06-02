// serial.cpp - copia dell'originale 128x64_Flexible/serial.cpp
// Nessuna modifica necessaria per LOLIN32 Lite
// File placeholder: PlatformIO compilera' la versione originale tramite build_src_filter
// Vedi platformio.ini: build_src_filter per 128x64_LOLIN32
#include "serial.h"

String logotype = R"(
 ____   ____   ___   _  _     __  __  _  _  __   __  
| __ ) / ___| / _ \ | || |   |  \/  || || | \ \ / /  
|  _ \|  ___|| | | || || |_  | |\/| || || |_ \ V /   
| |_) || |   | |_| ||__   _| | |  | ||__   _| | |    
nRF24+LOLIN32 Lite port - v3.2.0
)";

bool SerialCommands()
{
  if ( Serial.available() )
  {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    if ( command == "stop" || command == "s" )
      return false;
    CommandsHandler(command, false);
  }
  return true;
}

void CommandsHandler(String command, bool fromWeb)
{
  // Implementazione minima - vedi serial.cpp originale per la versione completa
  if ( command == "help" )
  {
    Serial.println("Commands: stop, help");
  }
}
