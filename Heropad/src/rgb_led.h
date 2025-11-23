/*
 * Heropad - RGB Status LED Control
 * Multi-board support with compile-time LED type selection
 *
 * Controls RGB LED to show BLE connection status:
 * - GREEN: Connected to Heroman game
 * - RED: Disconnected / waiting to connect
 * - BLUE: Calibrating joystick
 * - OFF: Startup / initialization
 *
 * Supported LED types:
 * - GPIO-based RGB LED (ESP32-WROOM-32): Common Cathode/Anode RGB LED
 * - WS2812 (ESP32-S3): Onboard addressable RGB LED (NeoPixel)
 */

#pragma once
#include <Arduino.h>
#include "pin_config.h"

// ============================================================================
// WS2812 (NeoPixel) Support for ESP32-S3
// ============================================================================
#ifdef RGB_LED_TYPE_WS2812
    #include <Adafruit_NeoPixel.h>

    // Create NeoPixel object
    Adafruit_NeoPixel rgbLed(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

    void initRgbLed() {
        rgbLed.begin();
        rgbLed.setBrightness(50);  // Set brightness to 20% (0-255)
        rgbLed.clear();
        rgbLed.show();
    }

    void setRgbColor(uint8_t r, uint8_t g, uint8_t b) {
        rgbLed.setPixelColor(0, rgbLed.Color(r, g, b));
        rgbLed.show();
    }

    // Convenience functions for common colors
    void rgbOff() {
        setRgbColor(0, 0, 0);
    }

    void rgbRed() {
        setRgbColor(255, 0, 0);
    }

    void rgbGreen() {
        setRgbColor(0, 255, 0);
    }

    void rgbBlue() {
        setRgbColor(0, 0, 255);
    }

    void rgbYellow() {
        setRgbColor(255, 255, 0);
    }

    void rgbCyan() {
        setRgbColor(0, 255, 255);
    }

    void rgbMagenta() {
        setRgbColor(255, 0, 255);
    }

    void rgbWhite() {
        setRgbColor(255, 255, 255);
    }

// ============================================================================
// GPIO-based RGB LED Support for ESP32-WROOM-32
// ============================================================================
#elif defined(RGB_LED_TYPE_GPIO)
    // LED logic levels (change if using common anode)
    #define LED_ON  HIGH    // For common cathode: HIGH = on
    #define LED_OFF LOW     // For common cathode: LOW = off

    // If using common anode RGB LED, uncomment these instead:
    // #define LED_ON  LOW
    // #define LED_OFF HIGH

    void initRgbLed() {
        // Configure RGB pins as outputs
        pinMode(RGB_LED_RED_PIN, OUTPUT);
        pinMode(RGB_LED_GREEN_PIN, OUTPUT);
        pinMode(RGB_LED_BLUE_PIN, OUTPUT);

        // Start with LED off
        digitalWrite(RGB_LED_RED_PIN, LED_OFF);
        digitalWrite(RGB_LED_GREEN_PIN, LED_OFF);
        digitalWrite(RGB_LED_BLUE_PIN, LED_OFF);
    }

    void setRgbColor(bool red, bool green, bool blue) {
        digitalWrite(RGB_LED_RED_PIN, red ? LED_ON : LED_OFF);
        digitalWrite(RGB_LED_GREEN_PIN, green ? LED_ON : LED_OFF);
        digitalWrite(RGB_LED_BLUE_PIN, blue ? LED_ON : LED_OFF);
    }

    // Convenience functions for common colors
    void rgbOff() {
        setRgbColor(false, false, false);
    }

    void rgbRed() {
        setRgbColor(true, false, false);
    }

    void rgbGreen() {
        setRgbColor(false, true, false);
    }

    void rgbBlue() {
        setRgbColor(false, false, true);
    }

    void rgbYellow() {
        setRgbColor(true, true, false);
    }

    void rgbCyan() {
        setRgbColor(false, true, true);
    }

    void rgbMagenta() {
        setRgbColor(true, false, true);
    }

    void rgbWhite() {
        setRgbColor(true, true, true);
    }
#else
    #error "No RGB LED type defined! Check pin_config.h"
#endif

// ============================================================================
// Common Status Indicator Functions (All board types)
// ============================================================================

void showConnected() {
    rgbGreen();  // Green = Connected
}

void showDisconnected() {
    rgbRed();    // Red = Disconnected
}

void showCalibrating() {
    rgbBlue();   // Blue = Calibrating
}
