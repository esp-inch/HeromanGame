#pragma once
#include <stdint.h>

const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds

struct Button {
    // state variables
    uint8_t     stateIndex;
    uint8_t     gpio_pin;
    int         lastReading;
    uint32_t    lastDebounceTime;
    uint16_t    state;

    // methods determining the logical state of the button
    bool pressed()    { processInput(); return state == 1; }
    bool released()   { processInput(); return state == 0xffff; }
    //bool held(uint16_t count = 0) { return state > 1 + count && state < 0xffff; }

    void processInput() {
        // reads the voltage on the pin connected to the button
        bool reading = digitalRead(gpio_pin);

        if (reading != lastReading) {
            lastDebounceTime = millis();
        }

        if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
            bool pressed = reading == HIGH;
            if (pressed) {

              if (state  < 0xfffe) 
                state++;
              else if (state == 0xfffe) 
                state = 2;

            } else if (state) {
                state = state == 0xffff ? 0 : 0xffff;
            }
        }

        lastReading = reading;
    }
};

// Button 			btnSelect       = { SELECT_BTN_STATE_INDEX, HIGH, 0, 0 };
