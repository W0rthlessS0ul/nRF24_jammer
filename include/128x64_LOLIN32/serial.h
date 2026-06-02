#pragma once
#include "config.h"

extern String logotype;

bool SerialCommands();
void CommandsHandler(String command, bool fromWeb);
