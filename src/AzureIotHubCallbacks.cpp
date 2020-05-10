#include "../include/AzureIotHubCallbacks.h"

// static members
bool AzureIotHubCallbacks::isConnected;
StaticJsonDocument<200> AzureIotHubCallbacks::deviceTwinJson;


void AzureIotHubCallbacks::executeCommand(const char *cmdName, char *payload) {
    if (strcmp(cmdName, "enableAlarm") == 0) {
        LOG_VERBOSE("Executing enableAlarm -> value: %s", payload);
        JFLAlarm::setAlarm(true);
    }
    else if (strcmp(cmdName, "disableAlarm") == 0) {
        LOG_VERBOSE("Executing disableAlarm -> value: %s", payload);
        JFLAlarm::setAlarm(false);
    }
}

void AzureIotHubCallbacks::configurePinout(const char* deviceTwinTag, char* deviceTwinPayload) {
    LOG_VERBOSE("Tag: %s. Payload => %s\r\n", deviceTwinTag, deviceTwinPayload);
    deserializeJson(AzureIotHubCallbacks::deviceTwinJson, deviceTwinPayload);

    JsonObject desiredPinout = AzureIotHubCallbacks::deviceTwinJson["desired"]["pinout"];
    uint8_t pinLED = desiredPinout["LED"];
    uint8_t pinSIN = desiredPinout["SIN"];
    uint8_t pinLIGA = desiredPinout["LIGA"];
    
    if(pinLIGA > 0 && pinLED > 0 && pinSIN > 0) {
        JFLAlarm::updatePinout(pinLED, pinSIN, pinLIGA);
    }
}

void AzureIotHubCallbacks::onEvent(const AzureIoTCallbacks_e cbType, const AzureIoTCallbackInfo_t *callbackInfo) {
    // ConnectionStatus callback
    if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
        LOG_VERBOSE("Is connected ? %s (%d)", callbackInfo->statusCode == AzureIoTConnectionOK ? "YES" : "NO",  callbackInfo->statusCode);
        AzureIotHubCallbacks::isConnected = callbackInfo->statusCode == AzureIoTConnectionOK;
        return;
    }

    // payload buffer doesn't have a null ending.
    // add null ending in another buffer before print
    AzureIOT::StringBuffer buffer;
    if (callbackInfo->payloadLength > 0) {
        buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
    }

    LOG_VERBOSE("[%s] event was received. Payload => %s\n", callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

    if (strcmp(callbackInfo->eventName, "SettingsUpdated") == 0) {
        configurePinout(callbackInfo->tag, *buffer);
    }

    if (strcmp(callbackInfo->eventName, "Command") == 0) {
        LOG_VERBOSE("Command name was => %s\r\n", callbackInfo->tag);
        executeCommand(callbackInfo->tag, *buffer);
    }
}

static void onEvent(const AzureIoTCallbacks_e cbType, const AzureIoTCallbackInfo_t *callbackInfo) {
        // payload buffer doesn't have a null ending.
    // add null ending in another buffer before print
    AzureIOT::StringBuffer buffer;
    if (callbackInfo->payloadLength > 0) {
        buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
    }

    LOG_ERROR("An error occurred: %s. Payload => %s\r\n", callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

}
