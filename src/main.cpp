#include <Arduino.h>
#include "led.h"
#include "web.h"

bool connectToWifi() {
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
}

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println("======== Arduino Hyper Cube ========");

  ledInitOnCore(0);

  if (connectToWifi()) {
    webServerInit(); // starts on core from CONFIG_ASYNC_TCP_RUNNING_CORE
  }
}

void loop() {
  // nothing todo here.
}
