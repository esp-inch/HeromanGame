# BLEGamepad to Heroman Button Mapping

## Overview
This guide maps BLEGamepad library buttons to what Heroman expects from Bluepad32.

## Button Mapping Table

| Heroman Action | Game Function | BLEGamepad Code |
|----------------|---------------|-----------------|
| **D-Pad Up** | Move up (unused in current game) | `bleGamepad.setHat1(HAT_UP);` |
| **D-Pad Down** | Squat/crouch | `bleGamepad.setHat1(HAT_DOWN);` |
| **D-Pad Left** | Move left | `bleGamepad.setHat1(HAT_LEFT);` |
| **D-Pad Right** | Move right | `bleGamepad.setHat1(HAT_RIGHT);` |
| **A Button** | High punch | `bleGamepad.press(BUTTON_1);` |
| **B Button** | Low punch | `bleGamepad.press(BUTTON_2);` |
| **X Button** | (Reserved) | `bleGamepad.press(BUTTON_3);` |
| **Y Button** | Triangle | `bleGamepad.press(BUTTON_4);` |
| **Select/Back** | Menu select | `bleGamepad.pressSelect();` |
| **Start** | Start game / Return to menu | `bleGamepad.pressStart();` |

## D-Pad Hat Values

```cpp
HAT_CENTERED      // Neutral position
HAT_UP            // Up only
HAT_UP_RIGHT      // Diagonal
HAT_RIGHT         // Right only
HAT_DOWN_RIGHT    // Diagonal
HAT_DOWN          // Down only (squat)
HAT_DOWN_LEFT     // Diagonal
HAT_LEFT          // Left only
HAT_UP_LEFT       // Diagonal
```

## Example: Basic Movement and Punch

```cpp
#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad("Heroman Controller", "ESP32-S3", 100);

void setup() {
    Serial.begin(115200);
    Serial.println("Heroman BLE Gamepad Starting...");
    bleGamepad.begin();
}

void loop() {
    if (bleGamepad.isConnected()) {
        // Example: Walk right and punch
        bleGamepad.setHat1(HAT_RIGHT);  // Move right
        delay(500);

        bleGamepad.press(BUTTON_1);     // High punch (A button)
        delay(100);
        bleGamepad.release(BUTTON_1);
        delay(400);

        // Stop moving
        bleGamepad.setHat1(HAT_CENTERED);
        delay(500);

        // Squat and low punch
        bleGamepad.setHat1(HAT_DOWN);   // Squat
        delay(200);
        bleGamepad.press(BUTTON_2);     // Low punch (B button)
        delay(100);
        bleGamepad.release(BUTTON_2);
        delay(300);

        bleGamepad.setHat1(HAT_CENTERED);
        delay(1000);
    } else {
        Serial.println("Waiting for Heroman to connect...");
        delay(1000);
    }
}
```

## Example: Interactive Controls via Serial

```cpp
#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad("Heroman Controller", "ESP32-S3", 100);

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Heroman BLE Controller ===");
    Serial.println("Commands:");
    Serial.println("  w/a/s/d - Move up/left/down/right");
    Serial.println("  j - High punch (A)");
    Serial.println("  k - Low punch (B)");
    Serial.println("  space - Center D-pad");
    Serial.println("  enter - Start button");
    bleGamepad.begin();
}

void loop() {
    if (bleGamepad.isConnected()) {
        if (Serial.available()) {
            char cmd = Serial.read();

            switch(cmd) {
                // D-Pad movement
                case 'w':
                    bleGamepad.setHat1(HAT_UP);
                    Serial.println("UP");
                    break;
                case 's':
                    bleGamepad.setHat1(HAT_DOWN);
                    Serial.println("DOWN (Squat)");
                    break;
                case 'a':
                    bleGamepad.setHat1(HAT_LEFT);
                    Serial.println("LEFT");
                    break;
                case 'd':
                    bleGamepad.setHat1(HAT_RIGHT);
                    Serial.println("RIGHT");
                    break;
                case ' ':
                    bleGamepad.setHat1(HAT_CENTERED);
                    Serial.println("CENTERED");
                    break;

                // Attack buttons
                case 'j':
                    bleGamepad.press(BUTTON_1);   // A button
                    Serial.println("HIGH PUNCH");
                    delay(100);
                    bleGamepad.release(BUTTON_1);
                    break;
                case 'k':
                    bleGamepad.press(BUTTON_2);   // B button
                    Serial.println("LOW PUNCH");
                    delay(100);
                    bleGamepad.release(BUTTON_2);
                    break;

                // Menu controls
                case '\n':
                case '\r':
                    bleGamepad.pressStart();
                    Serial.println("START");
                    delay(100);
                    bleGamepad.releaseStart();
                    break;
            }
        }
    } else {
        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 2000) {
            Serial.println("Waiting for Heroman connection...");
            lastMsg = millis();
        }
    }
    delay(10);
}
```

## Button Constants Reference

Based on standard BLE Gamepad HID specification:

```cpp
BUTTON_1   // A button (High Punch in Heroman)
BUTTON_2   // B button (Low Punch in Heroman)
BUTTON_3   // X button
BUTTON_4   // Y button
BUTTON_5   // L1/LB
BUTTON_6   // R1/RB
BUTTON_7   // L2/LT
BUTTON_8   // R2/RT
BUTTON_9   // SELECT/BACK
BUTTON_10  // START (also accessible via pressStart())
```

## Heroman Button Requirements Summary

**Essential for gameplay:**
- D-Pad LEFT/RIGHT: Movement
- D-Pad DOWN: Squat
- BUTTON_1 (A): High punch
- BUTTON_2 (B): Low punch
- START: Start game / Menu navigation

**Optional:**
- SELECT: Menu navigation
- BUTTON_3 (X): Reserved
- BUTTON_4 (Y): Reserved

## Troubleshooting

### Controller not connecting
1. Check ESP32-S3 has BLE enabled (not Classic Bluetooth)
2. Verify Heroman's Bluepad32 has BLE service enabled for S3
3. Put BLE gamepad in pairing mode before starting Heroman

### Buttons not responding
1. Verify button mapping matches Bluepad32 expectations
2. Check serial monitor for button press events
3. Ensure proper press/release timing (100ms recommended)

### Movement stuttering
1. Use HAT_CENTERED when releasing D-pad
2. Add small delays between rapid inputs
3. Avoid sending updates faster than 10ms

## Notes

- BLEGamepad library uses standard HID gamepad descriptor
- Bluepad32 should automatically recognize it as a generic BLE controller
- Maximum 16 buttons supported in standard HID gamepad mode
- D-Pad is Hat1 (first hat switch)
