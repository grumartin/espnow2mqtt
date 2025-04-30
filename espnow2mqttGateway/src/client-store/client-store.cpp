#include "client-store.h"
#include <algorithm> // For std::remove_if

// Helper function to convert MAC address to a string
String ClientStore::macToString(const uint8_t mac[6]) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

// Helper function to check if two MAC addresses are equal
bool ClientStore::macsEqual(const uint8_t* a, const uint8_t* b) {
    for (int i = 0; i < 6; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// Check if a client is already subscribed to a topic
bool ClientStore::isAlreadySubscribed(const String& topic, const uint8_t* mac) {
    if (clients.find(topic) == clients.end()) {
        return false;
    }

    for (const ESPClient& c : clients[topic]) {
        if (macsEqual(c.mac, mac)) {
            return true;
        }
    }
    return false;
}

// Get all clients subscribed to a topic
std::vector<ESPClient> ClientStore::getClientsForTopic(const String& topic) {
    if (clients.find(topic) != clients.end()) {
        return clients[topic];
    }
    return std::vector<ESPClient>();
}

// Add a client to a topic
void ClientStore::addClientToTopic(const String& topic, uint8_t mac[6]) {
    if (isAlreadySubscribed(topic, mac)) {
        Serial.printf("[INFO] Already subscribed to [%s]\n", topic.c_str());
        return;
    }

    ESPClient c;
    memcpy(c.mac, mac, 6);
    clients[topic].push_back(c);

    Serial.printf("[INFO] New subscription: [%s] for MAC [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                  topic.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Remove a client from a topic
void ClientStore::removeClientFromTopic(const String& topic, uint8_t mac[6]) {
    if (clients.find(topic) == clients.end()) {
        Serial.printf("[INFO] No clients subscribed to [%s]\n", topic.c_str());
        return;
    }

    auto& clientList = clients[topic];
    auto it = std::remove_if(clientList.begin(), clientList.end(),
                             [&mac](const ESPClient& c) {
                                 return memcmp(c.mac, mac, 6) == 0;
                             });
    if (it != clientList.end()) {
        clientList.erase(it, clientList.end());
        Serial.printf("[INFO] Removed client with MAC [%02X:%02X:%02X:%02X:%02X:%02X] from topic [%s]\n",
                      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], topic.c_str());
    } else {
        Serial.printf("[INFO] Client with MAC [%02X:%02X:%02X:%02X:%02X:%02X] not found in topic [%s]\n",
                      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], topic.c_str());
    }
}

// Clear all clients from a topic
void ClientStore::clearClientsFromTopic(const String& topic) {
    if (clients.find(topic) != clients.end()) {
        clients[topic].clear();
        Serial.printf("[INFO] Cleared all clients from topic [%s]\n", topic.c_str());
    } else {
        Serial.printf("[INFO] No clients to clear for topic [%s]\n", topic.c_str());
    }
}

// List all clients subscribed to a topic
void ClientStore::listClientsInTopic(const String& topic) {
    if (clients.find(topic) != clients.end()) {
        Serial.printf("[INFO] Clients subscribed to [%s]:\n", topic.c_str());
        for (const ESPClient& c : clients[topic]) {
            Serial.printf("  - [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                          c.mac[0], c.mac[1], c.mac[2], c.mac[3], c.mac[4], c.mac[5]);
        }
    } else {
        Serial.printf("[INFO] No clients subscribed to [%s]\n", topic.c_str());
    }
}