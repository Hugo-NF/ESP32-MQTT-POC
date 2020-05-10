#ifndef JFL_ALARM_H
#define JFL_ALARM_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Size of the JSON document (bytes)
#define MSG_SIZE 40

// Defining the number of the events
#define EVENTS_AMOUNT 2

// Defining the id of each event
#define SIN_PIN_EVENT_NO 0
#define LED_PIN_EVENT_NO 1

// ISRs
void IRAM_ATTR alarmRisingGPIOISR (void *arg);
void IRAM_ATTR alarmFallingGPIOISR (void *arg);

class JFLAlarm {

    public:
        // Static members
        static uint8_t pinLED;
        static uint8_t pinSIN;
        static uint8_t pinLIGA;


        static StaticJsonDocument<MSG_SIZE> messages[2];
        static bool pendingEvents[2];

        // Static methods
        static void defaultSetup();
        static void updatePinout(uint8_t newPinLED, uint8_t newPinSIN, uint8_t newPinLIGA);
        static void setAlarm(bool set);

};

#endif //JFL_ALARM_H