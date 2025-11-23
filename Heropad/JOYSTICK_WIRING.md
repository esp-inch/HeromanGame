# Heropad Wiring Guide - ESP32-WROOM-32
## KY-023 Joystick Shield Connection Reference

### Power Connections
```
IMPORTANT: Set the joystick shield voltage switch to 3.3V!

Joystick Shield       →  ESP32-WROOM-32
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
3.3V                  →  3.3V (Pin 1)
GND                   →  GND (Pin 14 or 32)
```

### Analog Connections (Joystick Axes)
```
Joystick Shield       →  ESP32-WROOM-32
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
A0 (X-Axis)           →  GPIO34 (Pin 5)
A1 (Y-Axis)           →  GPIO35 (Pin 6)
```

### Digital Connections (Buttons)
```
Joystick Shield       →  ESP32-WROOM-32
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
D2 (Button A)         →  GPIO25 (Pin 9)
D3 (Button B)         →  GPIO26 (Pin 10)
D4 (Button C)         →  GPIO27 (Pin 11)
D5 (Button D)         →  GPIO33 (Pin 8)  ⚠️ CHANGED - safer pin!
D6 (Button E)         →  GPIO4  (Pin 26) ⚠️ CHANGED - GPIO12 causes boot issues!
D7 (Button F)         →  GPIO16 (Pin 27) ⚠️ CHANGED - GPIO13 can cause boot issues!
D8 (Joystick K)       →  GPIO5  (Pin 29) ⚠️ CHANGED - GPIO32 had pull-up issues!
```

**IMPORTANT:** GPIO12, 13, 14 are strapping pins that can prevent the ESP32 from booting or uploading code when connected. The updated mapping avoids these problematic pins!

### RGB Status LED (Optional - shows BLE connection status)
```
External RGB LED      →  ESP32-WROOM-32
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
RED Anode (+)         →  GPIO23 (Pin 25) via 220Ω resistor
GREEN Anode (+)       →  GPIO22 (Pin 33) via 220Ω resistor
BLUE Anode (+)        →  GPIO21 (Pin 30) via 220Ω resistor
Common Cathode (-)    →  GND (Pin 14 or 32)
```

**LED Status Indicators:**
- 🔴 **RED**: Disconnected / waiting for BLE connection
- 🟢 **GREEN**: Connected to Heroman game via Bluetooth
- 🔵 **BLUE**: Calibrating joystick

**Hardware:** Use a common cathode RGB LED with three 220Ω current-limiting resistors (one for each color channel). If using a common anode LED, modify the LED_ON/LED_OFF definitions in [rgb_led.h](src/rgb_led.h).

## Complete Pin Mapping Table

| Function | Shield Pin | ESP32 GPIO | ESP32 Pin# | Notes |
|----------|-----------|------------|------------|-------|
| **Power** |
| VCC      | 3.3V      | 3.3V       | 1          | CRITICAL: Use 3.3V! |
| Ground   | GND       | GND        | 14/32      | Common ground |
| **Analog Inputs** |
| X-Axis   | A0        | GPIO34     | 5          | ADC1_CH6 |
| Y-Axis   | A1        | GPIO35     | 6          | ADC1_CH7 |
| **Button Inputs** |
| Button A | D2        | GPIO25     | 9          | High punch |
| Button B | D3        | GPIO26     | 10         | Low punch |
| Button C | D4        | GPIO27     | 11         | Extra |
| Button D | D5        | GPIO33     | 8          | Extra - SAFE PIN |
| Button E | D6        | GPIO4      | 26         | Extra - SAFE PIN (not GPIO12!) |
| Button F | D7        | GPIO16     | 27         | Extra - SAFE PIN (not GPIO13!) |
| Stick Btn| D8        | GPIO5      | 29         | Start/Menu - SAFE PIN (not GPIO32!) |
| **RGB Status LED (Optional)** |
| LED Red  | -         | GPIO23     | 25         | Via 220Ω resistor |
| LED Green| -         | GPIO22     | 33         | Via 220Ω resistor |
| LED Blue | -         | GPIO21     | 30         | Via 220Ω resistor |

## Wiring Tips

1. **Voltage Warning**: The joystick shield has a voltage selector switch. **MUST be set to 3.3V** before connecting to ESP32. Connecting 5V will damage your ESP32!

2. **Verify Pins**: Use a multimeter to verify the shield voltage before connecting.

3. **Connection Order**:
   - Connect all signal pins first
   - Connect GND second
   - Connect power (3.3V) last

4. **Cable Management**: Use color coding for wires:
   - Red = 3.3V
   - Black = GND
   - Yellow/Orange = Analog signals
   - Blue/Green = Digital signals

5. **Testing**: After wiring, upload the code and type `i` in the Serial Monitor to view joystick diagnostics before connecting any BLE gamepad.

## Button Mapping to Heroman Game

| Shield Button | BLE Gamepad | Heroman Action |
|---------------|-------------|----------------|
| Joystick Movement | D-Pad | Move/Walk |
| Button A      | Button 1    | High Punch |
| Button B      | Button 2    | Low Punch |
| Button C      | Button 3    | (Available) |
| Button D      | Button 4    | (Available) |
| Button E      | Button 5    | (Available) |
| Button F      | Button 6    | (Available) |
| Button K      | Start       | Start Game |

## Troubleshooting

### Cannot upload code / Boot loop / "invalid header" errors

**CAUSE:** GPIO12, GPIO13, or GPIO14 connected during upload

These are **strapping pins** that determine ESP32 boot mode:
- **GPIO12** - Flash voltage selector (CRITICAL!)
- GPIO13 - JTAG/Flash programming
- GPIO14 - JTAG/Flash programming

**SOLUTION:**
1. **Disconnect buttons from GPIO12, 13, 14** before uploading
2. Use the updated pin mapping above (GPIO4, GPIO16, GPIO33 instead)
3. If flash is corrupted, erase it: PlatformIO → Erase Flash
4. Re-upload firmware

This is why we moved:
- Button D: GPIO14 → **GPIO33** ✓
- Button E: GPIO12 → **GPIO4** ✓
- Button F: GPIO13 → **GPIO16** ✓

### Button not responding (especially joystick button K)

**CAUSE:** GPIO32 has unreliable internal pull-up resistors

Some ESP32 GPIO pins (especially GPIO32-39) have weak or unreliable internal pull-ups.

**SOLUTION:**
- Use GPIO5 instead (updated pin mapping above)
- GPIO5 has very reliable pull-ups and is commonly used for buttons

This is why we moved:
- Joystick Button K: GPIO32 → **GPIO5** ✓

### Joystick doesn't respond
- Check 3.3V power connection
- Verify GPIO pins are correctly wired
- Type `i` in Serial Monitor to see raw joystick values
- Try recalibrating with `c` command

### Wrong directional mapping
- Up/Down might be inverted - edit `joystick.h` lines for Y-axis
- Left/Right might be inverted - edit `joystick.h` lines for X-axis

### Buttons always pressed or never work
- Verify internal pull-ups are enabled (code does this automatically)
- Check button pins aren't shorted to GND
- Buttons are active-low (press = LOW voltage)

### Calibration issues
- Let joystick center for 2 seconds before calibrating
- Don't touch joystick during power-on calibration
- Manually recalibrate with `c` command in Serial Monitor

## Serial Monitor Commands

Once uploaded, you can use these commands for testing:

| Key | Action |
|-----|--------|
| `w` | Move Up (serial fallback) |
| `a` | Move Left (serial fallback) |
| `s` | Move Down (serial fallback) |
| `d` | Move Right (serial fallback) |
| `j` | High Punch (serial fallback) |
| `k` | Low Punch (serial fallback) |
| `i` | Show joystick diagnostics |
| `c` | Recalibrate joystick center |
| `h` | Show help |

**Note**: Serial commands only work when joystick is NOT being used (fallback mode).
