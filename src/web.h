#include <Arduino.h>
#include "config.h"
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <esp32/clk.h>
#include "settings.h"

AsyncWebServer server(80);
AsyncCallbackJsonWebHandler* jsonHandler = new AsyncCallbackJsonWebHandler("/settings");

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void webServerInit() {
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject root = response->getRoot().to<JsonObject>();

    root["bright"] = FastLED.getBrightness();
    root["change_period"] = CHANGE_PRD;
    root["currency_limit"] = CUR_LIMIT;
    root["led_data_pin"] = LED_DI;
    root["edge_leds_count"] = EDGE_LEDS;
    root["wifi_ssid"] = WIFI_SSID;
    root["cpu_freq"] = esp_clk_cpu_freq();
    root["heap_size"] = ESP.getHeapSize();
    root["free_heap"] = ESP.getFreeHeap();
    root["chip_model"] = ESP.getChipModel();
    root["chip_revision"] = ESP.getChipRevision();
    root["chip_cores"] = ESP.getChipCores();
    root["web_server_core_id"] = xPortGetCoreID();

    response->setLength();
    request->send(response);
  });

  jsonHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant& json) {
    JsonObject jsonObj = json.as<JsonObject>();

    uint8_t bright = jsonObj["bright"];

    Serial.println(bright);

    settings.bright = bright;
    FastLED.setBrightness(bright);
    memory.update();

    // AsyncJsonResponse* response = new AsyncJsonResponse();
    // JsonObject root = response->getRoot().to<JsonObject>();

    // response->setLength();
    request->send(204);
  });

  server.addHandler(jsonHandler);
  server.onNotFound(notFound);
  server.begin();
}