#include <Arduino.h>
#include <WiFiClientSecure.h>


#include "AzureIotHubCallbacks.h"
#include "Connectivity.h"
#include "env.h"
#include "JFLAlarm.h"
#include "OTAUpdate.h"


//Conn String format example: HostName=[BLA_BLA];DeviceId=[BLA_BLA];SharedAccessKey=[BLA_BLA]
AzureIoTConfig_t iotconfig(DEVICE_CONNECTION_STRING);

WiFiClientSecure wifiClient;
AzureIoTLiteClient iotclient(wifiClient);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting program...");

    // OTAUpdate::setup();

    if (!Connectivity::connectWiFi(WIFI_SSID, WIFI_PASS)) {
        while(1);
    }

    // Setting up pinout
    JFLAlarm::defaultSetup();

    // Add callbacks
    iotclient.setCallback(AzureIoTCallbackConnectionStatus, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackMessageSent, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackCommand, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackSettingsUpdated, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackError, AzureIotHubCallbacks::onError);

    iotclient.begin(&iotconfig);
}

unsigned long lastTick = 0, messagesSent = 0, eventIndex = 0;
char message[MSG_SIZE] = {0};
size_t bytesWritten;
bool sentSuccessfully = true;

void loop() {

    // OTAUpdate::listen();

    if (!iotclient.run()) {
        return; //No point to continue
    }

    unsigned long ms = millis();
    if (ms - lastTick > 15000) {  // Polling events every 10 seconds
        lastTick = ms;

        for(eventIndex = 0; eventIndex < EVENTS_AMOUNT; eventIndex++) {
            if (JFLAlarm::pendingEvents[eventIndex]) {
                bytesWritten = serializeJson(JFLAlarm::messages[eventIndex], message, MSG_SIZE);
                message[bytesWritten] = 0;
                sentSuccessfully = iotclient.sendTelemetry(message, bytesWritten);
                JFLAlarm::pendingEvents[eventIndex] = false;
                if(!sentSuccessfully) {
                    LOG_ERROR("Sending message \"%s\" has failed", message);
                    JFLAlarm::pendingEvents[eventIndex] = true;
                }
            }
            
        }
    }
}