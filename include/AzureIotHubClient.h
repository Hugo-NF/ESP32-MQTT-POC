#ifndef AZURE_IOT_HUB_CLIENT_H
#define AZURE_IOT_HUB_CLIENT_H

#include <Arduino.h>
#include <AzureIotHub.h>
#include <Esp32MQTTClient.h>

#include "Wifi.h"

class AzureIotHubClient {

    private:
        bool hasIotHubConnected = false;
        long messagesSent = 0;

    public:
        explicit AzureIotHubClient() = default;
        virtual ~AzureIotHubClient() = default;


        bool openIotHubConnection(const char* connectionString);
        bool sendMessageToHub(const char* message);
};

#endif