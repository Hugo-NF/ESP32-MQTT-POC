#include "../include/Wifi.h"

int Wifi::connect(const char* ssid, const char* password) {
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED);

    return WiFi.status();
}