#include <Arduino.h>
#include <WiFiClientSecure.h>


#include "AzureIotHubCallbacks.h"
#include "Connectivity.h"
#include "env.h"
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

    // Prepare LED on WROVER
    pinMode(LED_BUILTIN, OUTPUT);

    // Turn off all LEDs on WROVER
    digitalWrite(LED_BUILTIN, LOW);

    // Add callbacks
    iotclient.setCallback(AzureIoTCallbackConnectionStatus, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackMessageSent, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackCommand, AzureIotHubCallbacks::onEvent);
    iotclient.setCallback(AzureIoTCallbackSettingsUpdated, AzureIotHubCallbacks::onEvent);

    iotclient.begin(&iotconfig);
}

unsigned long lastTick = 0, loopId = 0;
void loop() {

    // OTAUpdate::listen();

    if (!iotclient.run()) {
        return; //No point to continue
    }

    unsigned long ms = millis();
    if (ms - lastTick > 15000) {  // send telemetry every 10 seconds
        char msg[64] = {0};
        int pos = 0;
        bool errorCode = false;

        lastTick = ms;

        // Randomize to simulate temperature
        long tempVal = random(35, 40);

        // Send telemetry
        if (tempVal >= 38) {
            pos = snprintf(msg, sizeof(msg) - 1, R"({"temperature": %.2f, "temperatureAlert": %d})", tempVal*1.0f, (tempVal >= 38));
            msg[pos] = 0;
            errorCode = iotclient.sendTelemetry(msg, pos);
        } else {
            pos = snprintf(msg, sizeof(msg) - 1, "{\"temperature\": %.2f}", tempVal*1.0f);
            msg[pos] = 0;
            errorCode = iotclient.sendTelemetry(msg, pos);
        }

        if (!errorCode) {
            LOG_ERROR("Sending message has failed");
        }

        if (!errorCode) {
            LOG_ERROR("Sending message has failed with error code %d", errorCode);
        }
    }
}