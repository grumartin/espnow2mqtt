#include <Arduino.h>
#include <ArduinoJson.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#endif
#include "ESPNowW.h"

uint8_t receiver_mac[] = {0x5C, 0xCF, 0x7F, 0xF0, 0x32, 0xD9};

void setup() {
    Serial.begin(9600);
    Serial.println("ESPNow sender Demo");
#ifdef ESP8266
    WiFi.mode(WIFI_STA);
#elif ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif
    WiFi.disconnect();
    ESPNow.init();
    ESPNow.add_peer(receiver_mac);
}

void loop() {
    static uint8_t counter = 0;

    JsonDocument doc;
    doc["msg"] = "hello from esp";
    doc["val"] = counter++;

    char buffer[100];
    size_t len = serializeJson(doc, buffer);

    ESPNow.send_message(receiver_mac, (uint8_t*)buffer, len + 1);

    Serial.print("Sent: ");
    Serial.println(buffer);

    delay(5000);
}