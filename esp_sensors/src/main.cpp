#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#endif

#include "ESPNowW.h"

uint8_t gateway_mac[] = {0x5C, 0xCF, 0x7F, 0xF0, 0x32, 0xD9};
const char* subscribed_topic = "test/topic";

void sendSubscription();
void sendSensorMessage(uint8_t counter);
void onReceive(uint8_t* mac, uint8_t* data, uint8_t len);

void setup() {
    Serial.begin(9600);
    Serial.println("🚀 ESPNow Client Starting");

#ifdef ESP8266
    WiFi.mode(WIFI_STA);
#elif ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif

    WiFi.disconnect();
    ESPNow.init();
    ESPNow.add_peer(gateway_mac);
    ESPNow.reg_recv_cb(onReceive);

    sendSubscription();
}

void loop() {
    static uint8_t counter = 0;

    sendSensorMessage(counter++);
    delay(5000);
}

void sendSubscription() {
    JsonDocument doc;
    doc["type"] = "subscribe";
    doc["topic"] = subscribed_topic;

    char buffer[128];
    size_t len = serializeJson(doc, buffer);

    ESPNow.send_message(gateway_mac, (uint8_t*)buffer, len + 1);

    Serial.print("📨 Sent subscription: ");
    Serial.println(buffer);
}

void sendSensorMessage(uint8_t counter) {
    JsonDocument doc;
    doc["msg"] = "hello from esp";
    doc["val"] = counter;
    doc["topic"] = subscribed_topic;

    char buffer[128];
    size_t len = serializeJson(doc, buffer);

    ESPNow.send_message(gateway_mac, (uint8_t*)buffer, len + 1);

    Serial.print("📤 Sent message: ");
    Serial.println(buffer);
}

void onReceive(uint8_t* mac, uint8_t* data, uint8_t len) {
    char message[256] = {0};
    memcpy(message, data, len);
    message[len] = '\0';

    Serial.println("📥 Received ESPNow message:");
    Serial.println(message);
}
