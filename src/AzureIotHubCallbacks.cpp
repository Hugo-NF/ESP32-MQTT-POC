#include "../include/AzureIotHubCallbacks.h"

// Class static members
bool AzureIotHubCallbacks::isConnected;

void AzureIotHubCallbacks::executeCommand(const char *cmdName, char *payload) {
    if (strcmp(cmdName, "setLED") == 0) {
        LOG_VERBOSE("Executing setLED -> value: %s", payload);
        int ledVal = atoi(payload);
        digitalWrite(LED_BUILTIN, ledVal);
    }
}

void AzureIotHubCallbacks::onEvent(const AzureIoTCallbacks_e cbType, const AzureIoTCallbackInfo_t *callbackInfo) {
    // ConnectionStatus
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

    if (strcmp(callbackInfo->eventName, "Command") == 0) {
        LOG_VERBOSE("Command name was => %s\r\n", callbackInfo->tag);
        executeCommand(callbackInfo->tag, *buffer);
    }
}
