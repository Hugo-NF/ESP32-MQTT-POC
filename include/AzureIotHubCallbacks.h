#ifndef AZURE_IOT_HUB_CALLBACKS_H
#define AZURE_IOT_HUB_CALLBACKS_H

#include "AzureIoTLiteClient.h"
#include "JFLAlarm.h"

class AzureIotHubCallbacks {

    private:
        static bool isConnected;
        static StaticJsonDocument<200> deviceTwinJson;
    
    public:
        static void executeCommand(const char *cmdName, char *payload);
        static void configurePinout(const char* deviceTwinTag, char* deviceTwinPayload);
        static void onEvent(const AzureIoTCallbacks_e cbType, const AzureIoTCallbackInfo_t *callbackInfo);
};

#endif //AZURE_IOT_HUB_CALLBACKS_H
