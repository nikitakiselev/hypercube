#include <Arduino.h>
#include <EncButton.h>
#include "led.h"
#ifdef WEB_SERVER_ENABLE
#include "web.h"
#endif

Button btn(BTN_PIN, INPUT_PULLDOWN);

void IRAM_ATTR buttonHandler() {
  btn.pressISR();
}

bool connectToWifi() {
#ifdef WEB_SERVER_ENABLE
  Serial.println("Trying to connect to WIFI.");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  uint8_t tries = 20;
  while (tries > 0 && WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    tries--;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection failed.");
  } else {
    Serial.println("Wifi connected successfully.");
    Serial.println(WiFi.localIP());
  }

  return WiFi.status() == WL_CONNECTED;
#endif
}

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println("======== Arduino Hyper Cube ========");

  print_wakeup_reason();
  esp_sleep_enable_ext0_wakeup(BTN_PIN, 0); // 1 = High, 0 = Low
  rtc_gpio_pullup_dis(BTN_PIN);
  rtc_gpio_pulldown_en(BTN_PIN);

  // analogReadResolution(10); // 10 bit, from 0 to 1023
  // analogSetWidth(10);
  // analogSetAttenuation(ADC_6db);
  // adcAttachPin(ADC_PIN);

  // attachInterrupt(digitalPinToInterrupt(BTN_PIN), buttonHandler, FALLING);
  // settingsInit();

  Serial.print("Bright: ");
  Serial.println(settings.bright);
  Serial.print("LED: ");
  Serial.println(settings.ledEnable ? "On" : "Off");
  Serial.print("Sound Mode: ");
  Serial.println(settings.soundMode ? "On" : "Off");

  pinMode(TRANSISTOR_PIN, OUTPUT);
  digitalWrite(TRANSISTOR_PIN, settings.powerState);

  ledInitOnCore(0);
#ifdef WEB_SERVER_ENABLE
  //if (connectToWifi()) {
  //  webServerInit(); // starts on core from CONFIG_ASYNC_TCP_RUNNING_CORE
  //}
#endif
}

void loop() {
  if (memory.tick()) Serial.println("Memory Updated!");

  btn.tick();

  // if (lastTransistorState != transistorState) {
  //   digitalWrite(TRANSISTOR_PIN, transistorState ? HIGH : LOW);
  //   lastTransistorState = transistorState;
  // }

  if (btn.hasClicks(1)) {
    settings.powerState = ! settings.powerState;

    Serial.printf("Current power state: %s\n", settings.powerState ? "On" : "Off");
    digitalWrite(TRANSISTOR_PIN, settings.powerState);

    if (settings.powerState == false) {
      esp_deep_sleep_start();
    }
  }

  if (btn.hold()) {
    Serial.println("Hold");
    ESP.restart();
  }

  // if (btn.hasClicks(2)) {
  //   settings.soundMode = ! settings.soundMode;
  //   Serial.print("Sound Mode: ");
  //   Serial.println(settings.soundMode ? "On" : "Off");
  //   memory.update();
  // }
}
