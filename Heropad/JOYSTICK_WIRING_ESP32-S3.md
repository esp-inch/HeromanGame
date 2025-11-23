# Heropad Wiring Guide - ESP32-S3
## KY-023 Joystick Shield Connection Reference for ESP32-S3-DevKitC-1

**Board**: ESP32-S3-DevKitC-1 (ESP32-S3-N8R2)
**Features**: 8MB Flash, 2MB PSRAM, Onboard WS2812 RGB LED

### Power Connections
```
IMPORTANT: Set the joystick shield voltage switch to 3.3V!

Joystick Shield       →  ESP32-S3-DevKitC-1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
3.3V                  →  3V3
GND                   →  GND
```

### Analog Connections (Joystick Axes)
```
Joystick Shield       →  ESP32-S3-DevKitC-1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
A0 (X-Axis)           →  GPIO1 (ADC1_CH0)
A1 (Y-Axis)           →  GPIO2 (ADC1_CH1)
```

**Note**: ESP32-S3 does NOT have GPIO34/GPIO35 like ESP32-WROOM! We use GPIO1/GPIO2 instead.

### Digital Connections (Buttons)
```
Joystick Shield       →  ESP32-S3-DevKitC-1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
D2 (Button A)         →  GPIO10
D3 (Button B)         →  GPIO11
D4 (Button C)         →  GPIO12
D5 (Button D)         →  GPIO13
D6 (Button E)         →  GPIO14
D7 (Button F)         →  GPIO21
D8 (Joystick K)       →  GPIO47
```

### RGB Status LED (Onboard - No wiring needed!)
```
Onboard WS2812        →  GPIO48 (pre-wired on board)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔴 RED                =  Disconnected / waiting for BLE connection
🟢 GREEN              =  Connected to Heroman game via Bluetooth
🔵 BLUE               =  Calibrating joystick
```

The ESP32-S3-DevKitC-1 has an addressable RGB LED (WS2812/NeoPixel) already on the board at GPIO48. **No external RGB LED wiring needed!** The code automatically detects the S3 and uses the onboard LED.

## Complete Pin Mapping Table

| Function | Shield Pin | ESP32-S3 GPIO | Notes |
|----------|-----------|---------------|-------|
| **Power** |
| VCC      | 3.3V      | 3V3           | CRITICAL: Use 3.3V! |
| Ground   | GND       | GND           | Common ground |
| **Analog Inputs** |
| X-Axis   | A0        | GPIO1         | ADC1_CH0 |
| Y-Axis   | A1        | GPIO2         | ADC1_CH1 |
| **Button Inputs** |
| Button A | D2        | GPIO10        | High punch |
| Button B | D3        | GPIO11        | Low punch |
| Button C | D4        | GPIO12        | Extra |
| Button D | D5        | GPIO13        | Extra |
| Button E | D6        | GPIO14        | Extra |
| Button F | D7        | GPIO21        | Extra |
| Stick Btn| D8        | GPIO47        | Start/Menu |
| **Onboard RGB LED** |
| WS2812   | (onboard) | GPIO48        | Pre-wired, no connection needed |

## Wiring Tips

1. **Voltage Warning**: The joystick shield has a voltage selector switch. **MUST be set to 3.3V** before connecting to ESP32-S3. Connecting 5V will damage your ESP32!

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

## Differences from ESP32-WROOM-32

The ESP32-S3 has a different GPIO layout than the original ESP32:

| Feature | ESP32-WROOM-32 | ESP32-S3 |
|---------|----------------|----------|
| Joystick X | GPIO34 | **GPIO1** |
| Joystick Y | GPIO35 | **GPIO2** |
| Button A-F | GPIO25,26,27,33,4,16 | **GPIO10,11,12,13,14,21** |
| Button K | GPIO5 | **GPIO47** |
| RGB LED | External (GPIO23,22,21) | **Onboard WS2812 (GPIO48)** |
| Strapping pins | GPIO0,2,12,15,46 | Different set |

## Building for ESP32-S3

To compile for ESP32-S3, use the correct PlatformIO environment:

```bash
# Build for ESP32-S3
pio run -e esp32-s3-devkitc-1

# Upload to ESP32-S3
pio run -e esp32-s3-devkitc-1 -t upload

# Monitor serial output
pio device monitor -b 115200
```

The code automatically detects ESP32-S3 at compile time and uses the correct pin mappings and WS2812 RGB LED driver.

## Troubleshooting

### Joystick doesn't respond
- Check 3.3V power connection
- Verify GPIO pins are correctly wired (GPIO1/GPIO2, NOT GPIO34/GPIO35!)
- Type `i` in Serial Monitor to see raw joystick values
- Try recalibrating with `c` command

### RGB LED not working
- The RGB LED is onboard - no external wiring needed
- LED is controlled via WS2812 protocol (Adafruit NeoPixel library)
- LED is dimmed to 20% brightness to avoid being blinding

### Buttons not responding
- Verify internal pull-ups are enabled (code does this automatically)
- Check button pins aren't shorted to GND
- Buttons are active-low (press = LOW voltage)

### Calibration issues
- Let joystick center for 2 seconds before calibrating
- Don't touch joystick during power-on calibration
- Manually recalibrate with `c` command in Serial Monitor

### Wrong board selected in PlatformIO
- Make sure you're building with `-e esp32-s3-devkitc-1`
- Check that `platformio.ini` has the `[env:esp32-s3-devkitc-1]` section
- Verify build output shows "ESP32-S3" target

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

## Hardware Specifications

**ESP32-S3-DevKitC-1 (N8R2 variant)**
- CPU: Dual-core Xtensa LX7 @ 240MHz
- Flash: 8MB
- PSRAM: 2MB
- WiFi: 802.11 b/g/n (2.4GHz)
- Bluetooth: BLE 5.0
- USB: Native USB OTG support
- RGB LED: WS2812 addressable LED on GPIO48
- GPIO: 45 programmable GPIOs
- ADC: 2x 12-bit SAR ADCs, up to 20 channels

**Advantages over ESP32-WROOM-32:**
- ✅ Faster CPU (240MHz vs 240MHz, but newer architecture)
- ✅ Better Bluetooth (BLE 5.0 vs BLE 4.2)
- ✅ More PSRAM (2MB vs 0MB on most ESP32 boards)
- ✅ Native USB support
- ✅ Onboard RGB LED (no external wiring!)
- ✅ More GPIO pins available

**Perfect for the BLE Gamepad project!** 🎮
