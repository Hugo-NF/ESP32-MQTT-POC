#include "../include/AzureIotHubClient.h"

bool AzureIotHubClient::openIotHubConnection(const char* connectionString) {

    if(WiFi.status() == WL_CONNECTED) {
        if(!Esp32MQTTClient_Init((const uint8_t*) connectionString)) {
            this->hasIotHubConnected = false;
        }
        this->hasIotHubConnected = true;
    } 
    return this->hasIotHubConnected;
}

bool AzureIotHubClient::sendMessageToHub(const char* message) {
    Serial.println(message);
    if(this->hasIotHubConnected && WiFi.status() == WL_CONNECTED){
        if(Esp32MQTTClient_SendEvent(message)) {
            this->messagesSent++;
            return true;
        }
    }
    return false;
}
