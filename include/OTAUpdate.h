#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <ArduinoOTA.h>

class OTAUpdate {

    public:

    // Constructor
    explicit OTAUpdate() = default;
    
    static void setup();
    static void listen();

};

#endif