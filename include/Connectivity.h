#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <Arduino.h>
#include <WiFi.h>

class Connectivity {

    public:
    static bool connectWiFi(const char* ssid, const char* password);

};

#endif //CONNECTIVITY_H