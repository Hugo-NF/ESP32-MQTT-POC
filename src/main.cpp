#include "../include/env.h"
#include "../include/OTAUpdate.h"
#include "../include/Wifi.h"

#include <Arduino.h>

//variabls for blinking an LED with Millis
const int led = LED_BUILTIN; // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 10000;  // interval at which to blink (milliseconds)
int ledState = LOW;  // ledState used to set the LED

void setup() {

    pinMode(led, OUTPUT);
    
    if(Wifi::connect(WIFI_SSID, WIFI_PASS) != WL_CONNECTED)
        ESP.restart();

    OTAUpdate::setup();
}

void loop() {
    OTAUpdate::listen();
  
    //loop to blink without delay
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        // if the LED is off turn it on and vice-versa:
        ledState = not(ledState);
        // set the LED with the ledState of the variable:
        digitalWrite(led,  ledState);
    }
}