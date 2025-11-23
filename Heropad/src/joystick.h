/*
 * Heropad - Joystick Input Handler
 * Reads KY-023 Joystick Shield analog axes and buttons
 */

#pragma once

#include <Arduino.h>
#include "pin_config.h"

// ============================================================================
// JOYSTICK STATE STRUCTURE
// ============================================================================

struct JoystickState {
    // Analog axes (raw values 0-4095)
    int16_t xRaw;
    int16_t yRaw;

    // Calibrated center offsets
    int16_t xCenter;
    int16_t yCenter;

    // Digital directional state
    bool up;
    bool down;
    bool left;
    bool right;

    // Button states (true = pressed)
    bool buttonA;
    bool buttonB;
    bool buttonC;
    bool buttonD;
    bool buttonE;
    bool buttonF;
    bool buttonK;

    // Initialization flag
    bool calibrated;
};

// Global joystick state
JoystickState joystick = {0};

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void calibrateJoystick();

// ============================================================================
// INITIALIZATION
// ============================================================================

void initJoystick() {
    // Configure analog pins (ADC1 channels)
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);

    // Configure button pins with internal pull-ups (Active LOW)
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    pinMode(BUTTON_B_PIN, INPUT_PULLUP);
    pinMode(BUTTON_C_PIN, INPUT_PULLUP);
    pinMode(BUTTON_D_PIN, INPUT_PULLUP);
    pinMode(BUTTON_E_PIN, INPUT_PULLUP);
    pinMode(BUTTON_F_PIN, INPUT_PULLUP);
    pinMode(BUTTON_K_PIN, INPUT_PULLUP);

    // Set ADC resolution to 12-bit (0-4095)
    analogReadResolution(12);

    // Set ADC attenuation to 11dB (0-3.3V full range)
    analogSetAttenuation(ADC_11db);

    Serial.println("Joystick pins configured");

    // Auto-calibrate center position
    calibrateJoystick();
}

// ============================================================================
// CALIBRATION
// ============================================================================

void calibrateJoystick() {
    Serial.println("\n=== JOYSTICK CALIBRATION ===");
    Serial.println("Release joystick to center position...");
    Serial.println("Calibrating in 2 seconds...");
    delay(2000);

    // Read multiple samples for stability - increased samples for accuracy
    const int samples = 50;
    int32_t xSum = 0;
    int32_t ySum = 0;

    Serial.print("Sampling");
    for (int i = 0; i < samples; i++) {
        xSum += analogRead(JOYSTICK_X_PIN);
        ySum += analogRead(JOYSTICK_Y_PIN);
        if (i % 10 == 0) Serial.print(".");
        delay(20);
    }
    Serial.println();

    joystick.xCenter = xSum / samples;
    joystick.yCenter = ySum / samples;
    joystick.calibrated = true;

    Serial.printf("✓ Calibration complete!\n");
    Serial.printf("  X-Center: %d (range: %d-%d)\n", joystick.xCenter,
                  joystick.xCenter - DEADZONE_RADIUS,
                  joystick.xCenter + DEADZONE_RADIUS);
    Serial.printf("  Y-Center: %d (range: %d-%d)\n", joystick.yCenter,
                  joystick.yCenter - DEADZONE_RADIUS,
                  joystick.yCenter + DEADZONE_RADIUS);
    Serial.printf("  Deadzone: ±%d\n", DEADZONE_RADIUS);

    // Show current readings
    delay(100);
    Serial.println("\nCurrent readings:");
    Serial.printf("  X: %d  Y: %d\n", analogRead(JOYSTICK_X_PIN), analogRead(JOYSTICK_Y_PIN));
    Serial.println();
}

// ============================================================================
// JOYSTICK READING
// ============================================================================

void readJoystick() {
    // Read analog axes
    joystick.xRaw = analogRead(JOYSTICK_X_PIN);
    joystick.yRaw = analogRead(JOYSTICK_Y_PIN);

    // Calculate relative positions from calibrated center
    int16_t xOffset = joystick.xRaw - joystick.xCenter;
    int16_t yOffset = joystick.yRaw - joystick.yCenter;

    // Apply deadzone and determine directional state
    if (abs(xOffset) < DEADZONE_RADIUS) {
        // Within deadzone - no horizontal movement
        joystick.left = false;
        joystick.right = false;
    } else {
        // Outside deadzone - determine direction
        joystick.left = (xOffset < 0);
        joystick.right = (xOffset > 0);
    }

    if (abs(yOffset) < DEADZONE_RADIUS) {
        // Within deadzone - no vertical movement
        joystick.up = false;
        joystick.down = false;
    } else {
        // Outside deadzone - determine direction
        // Y-axis inverted for this joystick shield
        joystick.up = (yOffset > 0);      // Pushing stick up = higher voltage
        joystick.down = (yOffset < 0);    // Pulling stick down = lower voltage
    }

    // Read buttons (Active LOW - pressed = LOW)
    joystick.buttonA = !digitalRead(BUTTON_A_PIN);
    joystick.buttonB = !digitalRead(BUTTON_B_PIN);
    joystick.buttonC = !digitalRead(BUTTON_C_PIN);
    joystick.buttonD = !digitalRead(BUTTON_D_PIN);
    joystick.buttonE = !digitalRead(BUTTON_E_PIN);
    joystick.buttonF = !digitalRead(BUTTON_F_PIN);
    joystick.buttonK = !digitalRead(BUTTON_K_PIN);
}

// ============================================================================
// DIAGNOSTIC OUTPUT
// ============================================================================

void printJoystickState() {
    // Calculate offsets from center
    int16_t xOffset = joystick.xRaw - joystick.xCenter;
    int16_t yOffset = joystick.yRaw - joystick.yCenter;

    Serial.print("Raw: X=");
    Serial.print(joystick.xRaw);
    Serial.print(" Y=");
    Serial.print(joystick.yRaw);

    Serial.print(" | Offset: X=");
    Serial.print(xOffset);
    Serial.print(" Y=");
    Serial.print(yOffset);

    Serial.print(" | Center: X=");
    Serial.print(joystick.xCenter);
    Serial.print(" Y=");
    Serial.print(joystick.yCenter);

    Serial.print(" | Dir: ");
    if (joystick.up) Serial.print("↑");
    if (joystick.down) Serial.print("↓");
    if (joystick.left) Serial.print("←");
    if (joystick.right) Serial.print("→");
    if (!joystick.up && !joystick.down && !joystick.left && !joystick.right) {
        Serial.print("● CENTERED");
    }

    Serial.print(" | Btns: ");
    if (joystick.buttonA) Serial.print("A ");
    if (joystick.buttonB) Serial.print("B ");
    if (joystick.buttonC) Serial.print("C ");
    if (joystick.buttonD) Serial.print("D ");
    if (joystick.buttonE) Serial.print("E ");
    if (joystick.buttonF) Serial.print("F ");
    if (joystick.buttonK) Serial.print("K ");
    if (!joystick.buttonA && !joystick.buttonB && !joystick.buttonC &&
        !joystick.buttonD && !joystick.buttonE && !joystick.buttonF && !joystick.buttonK) {
        Serial.print("(none)");
    }

    Serial.println();
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Check if any joystick input is active
bool isJoystickActive() {
    return (joystick.up || joystick.down || joystick.left || joystick.right ||
            joystick.buttonA || joystick.buttonB || joystick.buttonC ||
            joystick.buttonD || joystick.buttonE || joystick.buttonF || joystick.buttonK);
}

// Get current D-pad direction as HAT value (for BLE Gamepad library)
uint8_t getJoystickHatDirection() {
    // Combine directions into HAT position
    if (joystick.up && joystick.right) return HAT_UP_RIGHT;
    if (joystick.up && joystick.left) return HAT_UP_LEFT;
    if (joystick.down && joystick.right) return HAT_DOWN_RIGHT;
    if (joystick.down && joystick.left) return HAT_DOWN_LEFT;
    if (joystick.up) return HAT_UP;
    if (joystick.down) return HAT_DOWN;
    if (joystick.left) return HAT_LEFT;
    if (joystick.right) return HAT_RIGHT;

    return HAT_CENTERED;
}
