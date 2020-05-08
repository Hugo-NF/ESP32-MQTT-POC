#include <Arduino.h>
#include "AzureIoTLiteClient.h"
#include <WiFiClientSecure.h>
#include "env.h"
#include "OTAUpdate.h"

const char* ssidName = WIFI_SSID;      // your network SSID (name of wifi network)
const char* ssidPass = WIFI_PASS;      // your network password

//Conn String format example: HostName=[BLA_BLA];DeviceId=[BLA_BLA];SharedAccessKey=[BLA_BLA]
AzureIoTConfig_t iotconfig(DEVICE_CONNECTION_STRING);

WiFiClientSecure wifiClient;
AzureIoTLiteClient iotclient(wifiClient);
bool isConnected = false;

static bool connectWiFi() {
    Serial.printf("Connecting WIFI to SSID: %s\r\n", ssidName);

    WiFi.begin(ssidName, ssidPass);

    // Attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.printf("Connected to %s\r\n", ssidName);
    Serial.print("IP: "); Serial.println(WiFi.localIP());

    return true;
}

void executeCommand(const char *cmdName, char *payload) {
    if (strcmp(cmdName, "setLED") == 0) {
        LOG_VERBOSE("Executing setLED -> value: %s", payload);
        int ledVal = atoi(payload);
        digitalWrite(LED_BUILTIN, ledVal);
    }
}

void onEvent(const AzureIoTCallbacks_e cbType, const AzureIoTCallbackInfo_t *callbackInfo) {
    // ConnectionStatus
    if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
        LOG_VERBOSE("Is connected ? %s (%d)", callbackInfo->statusCode == AzureIoTConnectionOK ? "YES" : "NO",  callbackInfo->statusCode);
        isConnected = callbackInfo->statusCode == AzureIoTConnectionOK;
        return;
    }

    // payload buffer doesn't have a null ending.
    // add null ending in another buffer before print
    AzureIOT::StringBuffer buffer;
    if (callbackInfo->payloadLength > 0) {
        buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
    }

    LOG_VERBOSE("[%s] event was received. Payload => %s\n", callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

    if (strcmp(callbackInfo->eventName, "Command") == 0) {
        LOG_VERBOSE("Command name was => %s\r\n", callbackInfo->tag);
        executeCommand(callbackInfo->tag, *buffer);
    }
}

void setup() {
    delay(2000);
    Serial.begin(9600);
    Serial.println("It begins");

    if (!connectWiFi()) {
        while(1)
            ;
    }

    // Prepare LED on WROVER
    pinMode(LED_BUILTIN, OUTPUT);

    // Turn off all LEDs on WROVER
    digitalWrite(LED_BUILTIN, LOW);

    // Add callbacks
    iotclient.setCallback(AzureIoTCallbackConnectionStatus, onEvent);
    iotclient.setCallback(AzureIoTCallbackMessageSent, onEvent);
    iotclient.setCallback(AzureIoTCallbackCommand, onEvent);
    iotclient.setCallback(AzureIoTCallbackSettingsUpdated, onEvent);

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