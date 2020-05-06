#include "../include/AzureIotHubClient.h"
#include "../include/env.h"
#include "../include/OTAUpdate.h"

#include <Arduino.h>
#include <ArduinoJson.h>

AzureIotHubClient hub;
uint32_t pin = 32;
char message_content[255];

void IRAM_ATTR pinInterrupt(void *arg) {

    uint32_t* pin_num = (uint32_t*) arg;
    sniprintf(message_content, 255, "Salve caraio");
    hub.sendMessageToHub(message_content);
    Serial.println("Interrupção");
}

void setup() {
    Serial.begin(9600);

    if(Wifi::connect(WIFI_SSID, WIFI_PASS) != WL_CONNECTED)
        ESP.restart();

    if(hub.openIotHubConnection(DEVICE_CONNECTION_STRING))
        Serial.println("Conectado");

    pinMode(GPIO_NUM_19, INPUT);
    attachInterruptArg(GPIO_NUM_19, pinInterrupt, &pin, RISING);

    OTAUpdate::setup();
}

void loop() {
    
    OTAUpdate::listen();
}