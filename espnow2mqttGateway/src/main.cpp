#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>
#include "ESPNowW.h"
#include "config.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

struct Subscriber {
    uint8_t mac[6];
};

std::map<String, std::vector<Subscriber>> topicSubscribers;

void setupWiFi(const char* ssid, const char* password);
void connectToMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleESPNowMessage(uint8_t* mac, uint8_t* data, uint8_t len);
void forwardToMQTT(const char* topic, const char* message);
void forwardToESPNow(const String& topic, const char* message);
void registerSubscription(const uint8_t* mac, const String& topic);
bool isAlreadySubscribed(const String& topic, const uint8_t* mac);
bool macsEqual(const uint8_t* a, const uint8_t* b);

void setup() {
    Serial.begin(9600);
    Serial.println("🔌 Starting ESPNow-MQTT Gateway");

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    setupWiFi(WIFI_SSID, WIFI_PASSWORD);

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

    while (!mqttClient.connected() && attempt < maxRetries) {
        Serial.printf("Attempting MQTT connection (%d/%d)...\n", attempt + 1, maxRetries);

        bool connected = mqttClient.connect(MQTT_CLIENT_ID);

        if (connected) {
            Serial.println("✅ Connected to MQTT broker!");
            return;
        } else {
            Serial.printf("❌ Failed to connect. MQTT state: %d\n", mqttClient.state());
            delay(3000);
            attempt++;
        }
    }

    if (!mqttClient.connected()) {
        Serial.println("🛑 MQTT connection failed after max retries.");
    }
}

void handleESPNowMessage(uint8_t* mac, uint8_t* data, uint8_t len) {
    char message[256] = {0};
    memcpy(message, data, len);
    message[len] = '\0';

    JsonDocument doc;
    if (deserializeJson(doc, message)) {
        Serial.println("⚠️ JSON parse failed");
        return;
    }

    String type = doc["type"] | "";
    String topic = doc["topic"] | "";

    if (type == "subscribe" && topic != "") {
        registerSubscription(mac, topic);
        return;
    }

    if (topic != "") {
        forwardToMQTT(topic.c_str(), message);
    } else {
        forwardToMQTT("espnow/data", message);
    }
}

void forwardToMQTT(const char* topic, const char* message) {
    if (mqttClient.publish(topic, message)) {
        Serial.printf("📤 Sent to MQTT [%s]: %s\n", topic, message);
    } else {
        Serial.printf("❌ MQTT publish failed for topic [%s]\n", topic);
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char message[256];
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.printf("📥 MQTT -> ESPNow [%s]: %s\n", topic, message);
    forwardToESPNow(String(topic), message);
}

void forwardToESPNow(const String& topic, const char* message) {
    if (topicSubscribers.find(topic) == topicSubscribers.end()) {
        Serial.printf("⚠️ No ESPNow subscribers for topic [%s]\n", topic.c_str());
        return;
    }

    for (const Subscriber& sub : topicSubscribers[topic]) {
        ESPNow.send_message((uint8_t*)sub.mac, (uint8_t*)message, strlen(message) + 1);
        Serial.printf("📨 Forwarded to ESP [%02X:%02X:%02X:%02X:%02X:%02X]\n",
            sub.mac[0], sub.mac[1], sub.mac[2], sub.mac[3], sub.mac[4], sub.mac[5]);
    }
}

void registerSubscription(const uint8_t* mac, const String& topic) {
    if (isAlreadySubscribed(topic, mac)) {
        Serial.printf("🔁 Already subscribed to [%s]\n", topic.c_str());
        return;
    }

    Subscriber s;
    memcpy(s.mac, mac, 6);
    topicSubscribers[topic].push_back(s);
    mqttClient.subscribe(topic.c_str());

    Serial.printf("✅ New subscription: [%s] for MAC [%02X:%02X:%02X:%02X:%02X:%02X]\n",
        topic.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

bool isAlreadySubscribed(const String& topic, const uint8_t* mac) {
    for (const Subscriber& s : topicSubscribers[topic]) {
        if (macsEqual(s.mac, mac)) {
            return true;
        }
    }
    return false;
}

bool macsEqual(const uint8_t* a, const uint8_t* b) {
    for (int i = 0; i < 6; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}