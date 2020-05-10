#include "../include/Connectivity.h"

bool Connectivity::connectWiFi(const char* ssid, const char* password) {
    Serial.printf("Connecting WIFI to SSID: %s\r\n", ssid);

    WiFi.begin(ssid, password);

    // Attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.printf("Connected to %s\r\n", ssid);
    Serial.print("IP: "); Serial.println(WiFi.localIP());

    return true;
}