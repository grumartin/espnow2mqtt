#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>
#include "ESPNowW.h"
#include "config.h"
#include "client-store/client-store.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

ClientStore clientStore;

void setupWiFi(const char* ssid, const char* password);
void connectToMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleESPNowMessage(uint8_t* mac, uint8_t* data, uint8_t len);
void forwardToMQTT(const char* topic, const char* message);
void forwardToESPNow(const String& topic, const char* message);
void setup();
void loop();

void setup() {
    Serial.begin(9600);
    Serial.println("[INFO] Starting ESPNow-MQTT Gateway");

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    setupWiFi(WIFI_SSID, WIFI_PASSWORD);

    clientStore = ClientStore();
    Serial.println("[INFO] ClientStore initialized");

    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    connectToMQTT();

    ESPNow.init();
    ESPNow.reg_recv_cb(handleESPNowMessage);
}

void loop() {
    mqttClient.loop(); // Handle MQTT events
}

void setupWiFi(const char* ssid, const char* password) {
  Serial.println("[INFO] Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) { 
      delay(500);
      Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
      Serial.println("[INFO] WiFi Connected!");
      Serial.println("[INFO] IP Address: ");
      Serial.println(WiFi.localIP());
  } else {
      Serial.println("[ERROR] Failed to connect to WiFi");
  }
}

void connectToMQTT() {
    const int maxRetries = 5;
    int attempt = 0;

    while (!mqttClient.connected() && attempt < maxRetries) {
        Serial.printf("[INFO] Attempting MQTT connection (%d/%d)...\n", attempt + 1, maxRetries);

        bool connected = mqttClient.connect(MQTT_CLIENT_ID);

        if (connected) {
            Serial.println("[INFO] Connected to MQTT broker!");
            return;
        } else {
            Serial.printf("[ERROR] Failed to connect. MQTT state: %d\n", mqttClient.state());
            delay(3000);
            attempt++;
        }
    }

    if (!mqttClient.connected()) {
        Serial.println("[ERROR] MQTT connection failed after max retries.");
    }
}

void handleESPNowMessage(uint8_t* mac, uint8_t* data, uint8_t len) {
    char message[256] = {0};
    memcpy(message, data, len);
    message[len] = '\0';

    JsonDocument doc;
    if (deserializeJson(doc, message)) {
        Serial.println("[ERROR] JSON parse failed");
        return;
    }

    String type = doc["type"] | "";
    String topic = doc["topic"] | "";

    if (type == "subscribe" && topic != "") {
        clientStore.addClientToTopic(topic, mac);
        mqttClient.subscribe(topic.c_str());
        
        return;
    }

    if (type == "unsubscribe" && topic != "") {
        clientStore.removeClientFromTopic(topic, mac);
        std::vector<ESPClient> clients = clientStore.getClientsForTopic(topic);
        if (clients.empty()) {
            mqttClient.unsubscribe(topic.c_str());
        }
        return;
    }

    if (type == "publish" && topic != "") {
        forwardToESPNow(topic, message);
        forwardToMQTT(topic.c_str(), message);
        return;
    }

    return;
}

void forwardToMQTT(const char* topic, const char* message) {
    if (mqttClient.publish(topic, message)) {
        Serial.printf("[INFO] Sent to MQTT [%s]: %s\n", topic, message);
    } else {
        Serial.printf("[ERROR] MQTT publish failed for topic [%s]\n", topic);
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char message[256];
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.printf("[INFO] MQTT -> ESPNow [%s]: %s\n", topic, message);
    forwardToESPNow(String(topic), message);
}

void forwardToESPNow(const String& topic, const char* message) {
    if (clientStore.getClientsForTopic(topic).empty()) {
        Serial.printf("[INFO] No ESPNow subscribers for topic [%s]\n", topic.c_str());
        return;
    }

    for (ESPClient& client : clientStore.getClientsForTopic(topic)) {
        ESPNow.send_message((uint8_t*)client.mac, (uint8_t*)message, strlen(message) + 1);
        Serial.printf("[INFO] Forwarded to ESP [%02X:%02X:%02X:%02X:%02X:%02X]\n",
            client.mac[0], client.mac[1], client.mac[2], client.mac[3], client.mac[4], client.mac[5]);
    }
};