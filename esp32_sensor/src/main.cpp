#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "ESPNowW.h"

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
uint8_t receiver_mac[] = {0x5C, 0xCF, 0x7F, 0xF0, 0x32, 0xD9};

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  dht.begin();
  ESPNow.init();
  ESPNow.add_peer(receiver_mac);
}

void loop() {
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);
  float heatIndexC = dht.computeHeatIndex(tempC, humidity, false);
  float heatIndexF = dht.computeHeatIndex(tempF, humidity);

  if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(5000);
    return;
  }

  JsonDocument doc;
  doc["topic"] = "esp32/dht22";
  doc["temperature_c"] = tempC;
  doc["temperature_f"] = tempF;
  doc["humidity"] = humidity;
  doc["heat_index_c"] = heatIndexC;
  doc["heat_index_f"] = heatIndexF;

  char buffer[200];
  size_t len = serializeJson(doc, buffer);

  ESPNow.send_message(receiver_mac, (uint8_t*)buffer, len + 1);

  Serial.print("Sent: ");
  Serial.println(buffer);

  delay(5000);
}
