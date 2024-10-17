#pragma once

#include <EEManager.h>

struct Settings { 
  bool powerState = true;
  bool ledEnable = true;
  uint8_t bright = 100;
  bool soundMode = false;
};

Settings settings;
EEManager memory(settings);

void settingsInit() {
  EEPROM.begin(memory.blockSize());
  memory.begin(0, 'b');
}