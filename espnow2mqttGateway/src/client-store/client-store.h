#ifndef CLIENT_STORE_H
#define CLIENT_STORE_H

#include <Arduino.h>
#include <map>
#include <vector>

struct ESPClient {
    uint8_t mac[6];
};

class ClientStore {
    private:
        std::map<String, std::vector<ESPClient>> clients;
        String macToString(const uint8_t mac[6]);
        bool isAlreadySubscribed(const String& topic, const uint8_t* mac);
        bool macsEqual(const uint8_t* a, const uint8_t* b);

    public:
        std::vector<ESPClient> getClientsForTopic(const String& topic);
        void addClientToTopic(const String& topic, uint8_t mac[6]);
        void removeClientFromTopic(const String& topic, uint8_t mac[6]);
        void clearClientsFromTopic(const String& topic);
        void listClientsInTopic(const String& topic);
};

#endif