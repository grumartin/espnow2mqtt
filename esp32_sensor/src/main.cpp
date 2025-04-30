#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "ESPNowW.h"

// DHT Setup
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// LED pins
#define GREEN_LED_PIN 12
#define RED_LED_PIN 14

// Gateway MAC address
uint8_t gatewayMac[] = {0x5C, 0xCF, 0x7F, 0xF0, 0x32, 0xD9};

void handleIncomingMessage(const uint8_t* data, int data_len);
void onReceive(const uint8_t* mac_addr, const uint8_t* data, int data_len);
void sendJson(const JsonDocument& doc);
void sendSubscription();
void sendSensorData();
void printMacAddress();

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  dht.begin();

  // LED setup
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  printMacAddress();

  // ESP-NOW init
  ESPNow.init();
  ESPNow.add_peer(gatewayMac);
  ESPNow.reg_recv_cb(onReceive);
  sendSubscription();
}

void loop() {
  delay(5000);
  sendSensorData();
}

void sendSubscription() {
  JsonDocument doc;
  doc["type"] = "subscribe";
  doc["topic"] = "led/control";
  sendJson(doc);

  Serial.println("Sent subscription to 'led/control'");
}

void sendSensorData() {
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);
  float heatIndexC = dht.computeHeatIndex(tempC, humidity, false);
  float heatIndexF = dht.computeHeatIndex(tempF, humidity);

  if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  JsonDocument doc;
  doc["type"] = "publish";
  doc["topic"] = "esp32/dht22";
  doc["temperature_c"] = tempC;
  doc["temperature_f"] = tempF;
  doc["humidity"] = humidity;
  doc["heat_index_c"] = heatIndexC;
  doc["heat_index_f"] = heatIndexF;

  sendJson(doc);
}

void sendJson(const JsonDocument& doc) {
  char buffer[250];
  size_t len = serializeJson(doc, buffer);
  ESPNow.send_message(gatewayMac, (uint8_t*)buffer, len + 1);

  Serial.print("Sent: ");
  Serial.println(buffer);
}

void handleIncomingMessage(const uint8_t* data, int data_len) {
  char buffer[250];
  memcpy(buffer, data, data_len);
  buffer[data_len] = '\0';

  Serial.print("Received message: ");
  Serial.println(buffer);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buffer);
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  if (!doc["topic"].is<const char*>() || !doc["body"].is<const char*>()) return;

  String topic = doc["topic"];
  String command = doc["body"];

  if (topic == "led/control") {
    if (command == "red/on") {
      digitalWrite(RED_LED_PIN, HIGH);
    } else if (command == "red/off") {
      digitalWrite(RED_LED_PIN, LOW);
    } else if (command == "green/on") {
      digitalWrite(GREEN_LED_PIN, HIGH);
    } else if (command == "green/off") {
      digitalWrite(GREEN_LED_PIN, LOW);
    }
  }
}

void onReceive(const uint8_t* mac_addr, const uint8_t* data, int data_len){
  handleIncomingMessage(data, data_len);
}

void printMacAddress() {
  Serial.print("This device MAC address: ");
  Serial.println(WiFi.macAddress());
}