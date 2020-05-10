#include "../include/JFLAlarm.h"

// Static members
uint8_t JFLAlarm::pinLED;
uint8_t JFLAlarm::pinSIN;
uint8_t JFLAlarm::pinLIGA;
StaticJsonDocument<MSG_SIZE> JFLAlarm::messages[2];
bool JFLAlarm::pendingEvents[2];


// ISR's
void IRAM_ATTR alarmRisingGPIOISR (void *arg) {

    uint8_t* pinNumber = (uint8_t *) arg;
    if(*pinNumber == JFLAlarm::pinLED) {
        if(JFLAlarm::messages[LED_PIN_EVENT_NO]["EventLED"].set("Enabled")) {
            JFLAlarm::pendingEvents[LED_PIN_EVENT_NO] = true;
        }
    }
    else if(*pinNumber == JFLAlarm::pinSIN) {
        if(JFLAlarm::messages[SIN_PIN_EVENT_NO]["EventSIN"].set("Triggered")) {
            JFLAlarm::pendingEvents[SIN_PIN_EVENT_NO] = true;
        }
    }
}

void IRAM_ATTR alarmFallingGPIOISR (void *arg) {
    uint8_t* pinNumber = (uint8_t *) arg;
    if(*pinNumber == JFLAlarm::pinLED) {
        if(JFLAlarm::messages[LED_PIN_EVENT_NO]["EventLED"].set("Disabled")) {
            JFLAlarm::pendingEvents[LED_PIN_EVENT_NO] = true;
        }
    }
    else if(*pinNumber == JFLAlarm::pinSIN) {
        if(JFLAlarm::messages[SIN_PIN_EVENT_NO]["EventSIN"].set("Deactivated")) {
            JFLAlarm::pendingEvents[SIN_PIN_EVENT_NO] = true;
        }
    }
}


// Static methods
void JFLAlarm::defaultSetup() {

    // Initializes messages objects
    int index;
    for(index = 0; index < EVENTS_AMOUNT; index++) {
        JFLAlarm::pendingEvents[index] = false;
    }
    JFLAlarm::messages[SIN_PIN_EVENT_NO]["EventSIN"] = "undefined";
    JFLAlarm::messages[LED_PIN_EVENT_NO]["EventLED"] = "undefined";


    JFLAlarm::pinLED = GPIO_NUM_25;        
    JFLAlarm::pinSIN = GPIO_NUM_19;
    JFLAlarm::pinLIGA = GPIO_NUM_16;

    // pinModes
    pinMode(JFLAlarm::pinLIGA, OUTPUT);
    pinMode(JFLAlarm::pinLED, INPUT_PULLDOWN);
    pinMode(JFLAlarm::pinSIN, INPUT_PULLDOWN);

    digitalWrite(JFLAlarm::pinLIGA, HIGH);

    // Attaching interrupts
    attachInterruptArg(JFLAlarm::pinLED, alarmRisingGPIOISR, &JFLAlarm::pinLED, RISING);
    attachInterruptArg(JFLAlarm::pinSIN, alarmRisingGPIOISR, &JFLAlarm::pinSIN, RISING);
    attachInterruptArg(JFLAlarm::pinLED, alarmFallingGPIOISR, &JFLAlarm::pinLED, FALLING);
    attachInterruptArg(JFLAlarm::pinSIN, alarmFallingGPIOISR, &JFLAlarm::pinSIN, FALLING);
}

void JFLAlarm::updatePinout(uint8_t newPinLED=GPIO_NUM_25, uint8_t newPinSIN=GPIO_NUM_19, uint8_t newPinLIGA=GPIO_NUM_16) {

    if(newPinLED != JFLAlarm::pinLED) {
        Serial.printf("Changing LED Pin from pin %d to pin %d\r\n", JFLAlarm::pinLED, newPinLED);

        detachInterrupt(JFLAlarm::pinLED);
        pinMode(JFLAlarm::pinLED, INPUT);

        JFLAlarm::pinLED = newPinLED;

        pinMode(JFLAlarm::pinLED, INPUT_PULLDOWN);
        attachInterruptArg(JFLAlarm::pinLED, alarmRisingGPIOISR, &JFLAlarm::pinLED, RISING);
        attachInterruptArg(JFLAlarm::pinLED, alarmFallingGPIOISR, &JFLAlarm::pinLED, FALLING);        
    }

    if(newPinSIN != JFLAlarm::pinSIN) {
        Serial.printf("Changing SIN Pin from pin %d to pin %d\r\n", JFLAlarm::pinSIN, newPinSIN);

        detachInterrupt(JFLAlarm::pinSIN);
        pinMode(JFLAlarm::pinSIN, INPUT);

        JFLAlarm::pinSIN = newPinSIN;

        pinMode(JFLAlarm::pinSIN, INPUT_PULLDOWN);
        attachInterruptArg(JFLAlarm::pinSIN, alarmRisingGPIOISR, &JFLAlarm::pinSIN, RISING);
        attachInterruptArg(JFLAlarm::pinSIN, alarmFallingGPIOISR, &JFLAlarm::pinSIN, FALLING);

    }

    if(newPinLIGA != JFLAlarm::pinLIGA) {
        Serial.printf("Changing LIGA Pin from pin %d to pin %d\r\n", JFLAlarm::pinLIGA, newPinLIGA);
        // Disabling previous
        pinMode(JFLAlarm::pinLIGA, INPUT);
        JFLAlarm::pinLIGA = newPinLIGA;

        pinMode(JFLAlarm::pinLIGA, OUTPUT);
        digitalWrite(JFLAlarm::pinLIGA, HIGH);
    }
}

void JFLAlarm::setAlarm(bool set) {

    // Reading LED pin
    int currentState = digitalRead(JFLAlarm::pinLED);
    
    // Changing state (enabled and want to disable) or (disabled and and want to enable)
    if((currentState > 0 && !set) || (currentState == 0 && set)) {
        // 1 seg pulse
        digitalWrite(JFLAlarm::pinLIGA, LOW);

        delay(1000);

        digitalWrite(JFLAlarm::pinLIGA, HIGH);
    }
}