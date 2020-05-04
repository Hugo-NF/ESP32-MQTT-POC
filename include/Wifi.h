#include <Arduino.h>
#include <WiFi.h>


class Wifi {

    public:
    // Constructor
    explicit Wifi() = default;

    static int connect(const char* ssid, const char* password);

};