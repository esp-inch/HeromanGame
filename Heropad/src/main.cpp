/*
 * Heropad - BLE Gamepad Controller
 *
 * Dual-input gamepad for ESP32 to control Heroman game
 * Supports both KY-023 Joystick Shield AND Serial Monitor control
 *
 * HARDWARE: KY-023 Joystick Shield v1.A
 * See pin_config.h for wiring details
 *
 * Serial Commands (fallback/testing):
 *   Movement: w(up) a(left) s(down/squat) d(right) space(stop)
 *   Combat: j(high punch) k(low punch)
 *   Menu: enter(start)
 *   Debug: ? or h (help), i (joystick info)
 */

#include <Arduino.h>
#include <BleGamepad.h>
#include "joystick.h"
#include "rgb_led.h"

BleGamepad bleGamepad("Heropad", "ESP32", 100);

// Input mode tracking
enum InputMode {
    INPUT_NONE,
    INPUT_JOYSTICK,
    INPUT_SERIAL
};

InputMode lastInputMode = INPUT_NONE;

void setup() {
    Serial.begin(115200);
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║      Heropad Controller Ready!        ║");
    Serial.println("║   Dual Input: Joystick + Serial       ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();

    // Initialize joystick hardware
    Serial.println("Initializing joystick...");
    initJoystick();

    // Initialize RGB status LED
    Serial.println("Initializing status LED...");
    initRgbLed();
    showDisconnected();  // Start with red (disconnected)

    Serial.println("Controls:");
    Serial.println("  JOYSTICK:");
    Serial.println("    Stick        - Move / Squat");
    Serial.println("    Button A     - High Punch");
    Serial.println("    Button B     - Low Punch");
    Serial.println("    Button C-F   - Extra actions");
    Serial.println();
    Serial.println("  SERIAL MONITOR (fallback):");
    Serial.println("    [W] [A] [S] [D]  - Move / Squat");
    Serial.println("    [SPACE]          - Stop moving");
    Serial.println("    [J]              - High Punch");
    Serial.println("    [K]              - Low Punch");
    Serial.println("    [ENTER]          - Start Game");
    Serial.println("    [I]              - Joystick info");
    Serial.println();

    // Start BLE gamepad service
    bleGamepad.begin();
    Serial.println("Waiting for Heroman to connect...");
}

void loop() {
    static bool wasConnected = false;

    // Connection status
    if (bleGamepad.isConnected()) {
        if (!wasConnected) {
            Serial.println("\n✓ CONNECTED to Heroman!");
            Serial.println("Ready to play!\n");
            showConnected();  // Turn LED green
            wasConnected = true;
        }

        // Read joystick state every loop
        readJoystick();

        // Track if we're using joystick this cycle
        bool usingJoystick = isJoystickActive();

        // === JOYSTICK INPUT (Priority) ===
        if (usingJoystick) {
            // Show mode change indicator
            if (lastInputMode != INPUT_JOYSTICK) {
                Serial.println("[INPUT: Joystick]");
                lastInputMode = INPUT_JOYSTICK;
            }

            // Apply joystick direction to D-pad
            uint8_t hatDir = getJoystickHatDirection();
            bleGamepad.setHat1(hatDir);

            // Apply button states
            if (joystick.buttonA) {
                bleGamepad.press(BUTTON_1);
            } else {
                bleGamepad.release(BUTTON_1);
            }

            if (joystick.buttonB) {
                bleGamepad.press(BUTTON_2);
            } else {
                bleGamepad.release(BUTTON_2);
            }

            if (joystick.buttonC) {
                bleGamepad.press(BUTTON_3);
            } else {
                bleGamepad.release(BUTTON_3);
            }

            if (joystick.buttonD) {
                bleGamepad.press(BUTTON_4);
            } else {
                bleGamepad.release(BUTTON_4);
            }

            if (joystick.buttonE) {
                bleGamepad.press(BUTTON_5);
            } else {
                bleGamepad.release(BUTTON_5);
            }

            if (joystick.buttonF) {
                bleGamepad.press(BUTTON_6);
            } else {
                bleGamepad.release(BUTTON_6);
            }

            if (joystick.buttonK) {
                bleGamepad.pressStart();
            } else {
                bleGamepad.releaseStart();
            }
        }

        // === SERIAL MONITOR INPUT (Fallback/Testing) ===
        if (Serial.available()) {
            char cmd = Serial.read();

            // Show mode change indicator
            if (lastInputMode != INPUT_SERIAL && !usingJoystick) {
                Serial.println("[INPUT: Serial Monitor]");
                lastInputMode = INPUT_SERIAL;
            }

            switch(cmd) {
                // === MOVEMENT ===
                case 'w':
                case 'W':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_UP);
                        Serial.println("↑ UP");
                    }
                    break;

                case 's':
                case 'S':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_DOWN);
                        Serial.println("↓ DOWN (Squat)");
                    }
                    break;

                case 'a':
                case 'A':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_LEFT);
                        Serial.println("← LEFT");
                    }
                    break;

                case 'd':
                case 'D':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_RIGHT);
                        Serial.println("→ RIGHT");
                    }
                    break;

                case ' ':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_CENTERED);
                        Serial.println("● STOP");
                    }
                    break;

                // === COMBAT ===
                case 'j':
                case 'J':
                    if (!usingJoystick) {
                        bleGamepad.press(BUTTON_1);
                        Serial.println("👊 HIGH PUNCH");
                        delay(100);
                        bleGamepad.release(BUTTON_1);
                    }
                    break;

                case 'k':
                case 'K':
                    if (!usingJoystick) {
                        bleGamepad.press(BUTTON_2);
                        Serial.println("🦶 LOW PUNCH");
                        delay(100);
                        bleGamepad.release(BUTTON_2);
                    }
                    break;

                // === MENU ===
                case '\n':
                case '\r':
                    if (!usingJoystick) {
                        bleGamepad.pressStart();
                        Serial.println("▶ START");
                        delay(100);
                        bleGamepad.releaseStart();
                    }
                    break;

                // === COMBOS ===
                case 'q':
                case 'Q':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_DOWN);
                        delay(50);
                        bleGamepad.press(BUTTON_1);
                        Serial.println("🔽👊 SQUAT HIGH PUNCH");
                        delay(100);
                        bleGamepad.release(BUTTON_1);
                    }
                    break;

                case 'e':
                case 'E':
                    if (!usingJoystick) {
                        bleGamepad.setHat1(HAT_DOWN);
                        delay(50);
                        bleGamepad.press(BUTTON_2);
                        Serial.println("🔽🦶 SQUAT LOW PUNCH");
                        delay(100);
                        bleGamepad.release(BUTTON_2);
                    }
                    break;

                // === DEBUG/INFO ===
                case 'i':
                case 'I':
                    Serial.println("\n=== JOYSTICK STATUS ===");
                    printJoystickState();
                    Serial.println();
                    break;

                case 'h':
                case 'H':
                case '?':
                    Serial.println("\n=== CONTROLS ===");
                    Serial.println("JOYSTICK: Use stick + buttons A/B");
                    Serial.println("SERIAL: W/A/S/D + SPACE");
                    Serial.println("Combat: J (high) / K (low)");
                    Serial.println("Combos: Q (squat high) / E (squat low)");
                    Serial.println("Menu: ENTER (start)");
                    Serial.println("Info: I (joystick status)");
                    Serial.println();
                    break;

                case 'c':
                case 'C':
                    Serial.println("\n=== RECALIBRATING JOYSTICK ===");
                    showCalibrating();  // Turn LED blue during calibration
                    calibrateJoystick();
                    // Restore connection status color
                    if (bleGamepad.isConnected()) {
                        showConnected();
                    } else {
                        showDisconnected();
                    }
                    break;
            }
        }

    } else {
        // Disconnected
        if (wasConnected) {
            Serial.println("\n✗ DISCONNECTED from Heroman");
            Serial.println("Waiting to reconnect...");
            showDisconnected();  // Turn LED red
            wasConnected = false;
            lastInputMode = INPUT_NONE;
        }

        // Periodic status update
        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 3000) {
            Serial.print(".");
            lastMsg = millis();
        }
    }

    delay(10);  // Small delay to prevent serial spam
}
