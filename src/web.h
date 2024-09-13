#include <Arduino.h>
#include "config.h"
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

AsyncWebServer server(80);
void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void webServerInit() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject root = response->getRoot().to<JsonObject>();

    root["bright"] = BRIGHT;
    root["change_period"] = CHANGE_PRD;
    root["currency_limit"] = CUR_LIMIT;
    root["led_data_pin"] = LED_DI;
    root["edge_leds_count"] = EDGE_LEDS;
    root["wifi_ssid"] = WIFI_SSID;

    response->setLength();
    request->send(response);
  });

  server.onNotFound(notFound);
  server.begin();
}