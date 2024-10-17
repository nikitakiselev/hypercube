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

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println("======== Arduino Hyper Cube ========");

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
