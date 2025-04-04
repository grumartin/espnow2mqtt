#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ESPNowW.h"
#include "config.h"

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Function declarations
void setupWiFi(const char* ssid, const char* password);
void connectToMQTT();
void onRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len);
void sendToMQTT(const char* topic, const char* message);

void setup() {
    Serial.begin(9600);
    Serial.println("ESPNOW Gateway");

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    setupWiFi(WIFI_SSID, WIFI_PASSWORD);
    
    client.setServer(MQTT_BROKER, MQTT_PORT);
    connectToMQTT();
    
    ESPNow.init();
    ESPNow.reg_recv_cb(onRecv);
}

void loop() {
    client.loop(); // Handle MQTT events
}

void setupWiFi(const char* ssid, const char* password) {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) { 
      delay(500);
      Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n ✅ WiFi Connected!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
  } else {
      Serial.println("\n ❌ Failed to connect to WiFi");
  }
}

void connectToMQTT() {
    const int maxRetries = 5;
    int attempt = 0;

    while (!client.connected() && attempt < maxRetries) {
        Serial.printf("Attempting MQTT connection (%d/%d)...\n", attempt + 1, maxRetries);

        bool connected = client.connect(MQTT_CLIENT_ID);

        if (connected) {
            Serial.println("✅ Connected to MQTT broker!");
            return;
        } else {
            Serial.printf("❌ Failed to connect. MQTT state: %d\n", client.state());
            delay(3000);
            attempt++;
        }
    }

    if (!client.connected()) {
        Serial.println("🛑 MQTT connection failed after max retries.");
    }
}

void onRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    Serial.print("MAC ADDRESS of Sender: ");
    Serial.println(macStr);
    Serial.print("Data Received: ");

    char receivedMessage[250] = {0}; 
    memcpy(receivedMessage, data, data_len);
    receivedMessage[data_len] = '\0';

    Serial.println(receivedMessage);
    sendToMQTT("espnow/data", receivedMessage);
}

void sendToMQTT(const char* topic, const char* message) {
    if (client.publish(topic, message)) {
        Serial.println("Message sent to MQTT");
    } else {
        Serial.println("Failed to send message");
    }
}
