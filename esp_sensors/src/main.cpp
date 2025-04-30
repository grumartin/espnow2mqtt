#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#endif

#include "ESPNowW.h"

uint8_t gatewayMac[] = {0x5C, 0xCF, 0x7F, 0xF0, 0x32, 0xD9};

enum LedState {
    RED_ON,
    RED_OFF,
    GREEN_ON,
    GREEN_OFF
};

LedState currentState = RED_ON;

void onReceive(uint8_t* mac_addr, uint8_t* data, uint8_t data_len);
void sendJson(const JsonDocument& doc);
void sendSubscription();
void sendLedCommand();

void setup() {
    Serial.begin(9600);
    Serial.println("ESPNow Sender Demo");

#ifdef ESP8266
    WiFi.mode(WIFI_STA);
#elif ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif

    WiFi.disconnect();
    ESPNow.init();
    ESPNow.add_peer(gatewayMac);
    ESPNow.reg_recv_cb(onReceive);
    sendSubscription();
}

void loop() {
    delay(10000);
    sendLedCommand();
}

void sendSubscription() {
    JsonDocument doc;
    doc["type"] = "subscribe";
    doc["topic"] = "dht22/data";
    sendJson(doc);

    Serial.println("Sent subscription request for topic 'dht22/data'");
}

void sendLedCommand() {
    JsonDocument doc;
    doc["type"] = "publish";
    doc["topic"] = "led/control";

    switch (currentState) {
        case RED_ON:
            doc["body"] = "red/on";
            currentState = RED_OFF;
            break;
        case RED_OFF:
            doc["body"] = "red/off";
            currentState = GREEN_ON;
            break;
        case GREEN_ON:
            doc["body"] = "green/on";
            currentState = GREEN_OFF;
            break;
        case GREEN_OFF:
            doc["body"] = "green/off";
            currentState = RED_ON;
            break;
    }

    sendJson(doc);
}

void sendJson(const JsonDocument& doc) {
    char buffer[150];
    size_t len = serializeJson(doc, buffer);
    ESPNow.send_message(gatewayMac, (uint8_t*)buffer, len + 1);

    Serial.print("Sent: ");
    Serial.println(buffer);
}

void onReceive(uint8_t* mac_addr, uint8_t* data, uint8_t data_len) {
    char buffer[250];
    memcpy(buffer, data, data_len);
    buffer[data_len] = '\0';

    Serial.print("Received message: ");
    Serial.println(buffer);
}