#pragma once

#include <EEManager.h>

struct Settings { 
  bool powerState = false;
  bool ledEnable = true;
  uint8_t bright = 100;
  bool soundMode = false;
  uint32_t autoSleepMinutes = 60;
};

Settings settings;
EEManager memory(settings);

void settingsInit() {
  EEPROM.begin(memory.blockSize());
  memory.begin(0, 'b');
}