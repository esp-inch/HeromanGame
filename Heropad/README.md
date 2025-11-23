# Heropad

**BLE Gamepad Controller for Heroman Game**

Heropad is a dual-input BLE gamepad controller for ESP32 that lets you play the Heroman game using a physical KY-023 joystick shield or serial monitor commands.

## Features

- 🎮 **Dual Input Support**: Physical joystick + serial monitor fallback
- 🔵 **Multi-board Support**: ESP32-WROOM-32 and ESP32-S3-DevKitC-1
- 🟢 **RGB Status LED**: Visual BLE connection indicator (green = connected, red = disconnected)
- 📡 **Bluetooth LE**: Wireless gamepad communication
- 🎯 **Auto-calibration**: Joystick center point calibration on startup
- ⚙️ **Compile-time Configuration**: Automatic pin mapping based on board type

## Hardware Requirements

### Required
- **ESP32 Board** (one of):
  - ESP32-WROOM-32 (ESP32-DOIT-DEVKIT-V1, HiLetGo ESP32-32S)
  - ESP32-S3-DevKitC-1 (N8R2 variant with onboard RGB LED)
- **KY-023 Joystick Shield v1.A**
  - 2-axis analog joystick
  - 7 buttons (A, B, C, D, E, F, K)
  - Voltage selector (MUST be set to 3.3V)

### Optional
- **External RGB LED** (for ESP32-WROOM-32 only)
  - Common cathode RGB LED
  - Three 220Ω resistors
  - ESP32-S3 has onboard WS2812 RGB LED

## Quick Start

### 1. Hardware Setup

See detailed wiring guides:
- [ESP32-WROOM-32 Wiring](JOYSTICK_WIRING.md)
- [ESP32-S3 Wiring](JOYSTICK_WIRING_ESP32-S3.md)

**⚠️ CRITICAL**: Set joystick shield voltage switch to **3.3V** before connecting!

### 2. Software Setup

**Install PlatformIO:**
```bash
# Install PlatformIO Core
pip install platformio

# Or use PlatformIO IDE extension for VSCode
```

**Build and Upload:**

For ESP32-WROOM-32:
```bash
pio run -e esp32doit-devkit-v1 -t upload
```

For ESP32-S3:
```bash
pio run -e esp32-s3-devkitc-1 -t upload
```

**Monitor Serial Output:**
```bash
pio device monitor -b 115200
```

### 3. Usage

**Joystick Controls** (Primary):
- Analog stick → Movement / Squat
- Button A → High Punch
- Button B → Low Punch
- Button K → Start Game

**Serial Monitor Controls** (Fallback/Testing):
- `W/A/S/D` → Move up/left/down/right
- `SPACE` → Stop moving
- `J` → High Punch
- `K` → Low Punch
- `ENTER` → Start Game
- `I` → Show joystick diagnostics
- `C` → Recalibrate joystick
- `H` → Show help

**RGB LED Status**:
- 🔴 Red → Disconnected / waiting for BLE connection
- 🟢 Green → Connected to Heroman game
- 🔵 Blue → Calibrating joystick

## Project Structure

```
BLEGamepad/
├── platformio.ini                    # PlatformIO configuration
├── README.md                         # This file
├── JOYSTICK_WIRING.md               # ESP32-WROOM-32 wiring guide
├── JOYSTICK_WIRING_ESP32-S3.md      # ESP32-S3 wiring guide
└── src/
    ├── main.cpp                     # Main program loop
    ├── pin_config.h                 # Board-specific pin mappings
    ├── joystick.h                   # Joystick input handler
    └── rgb_led.h                    # RGB LED control (GPIO/WS2812)
```

## Board Support

### ESP32-WROOM-32
- **Analog pins**: GPIO34, GPIO35 (ADC1)
- **Button pins**: GPIO25, 26, 27, 33, 4, 16, 5
- **RGB LED**: External on GPIO23, 22, 21 (optional)

### ESP32-S3-DevKitC-1
- **Analog pins**: GPIO1, GPIO2 (ADC1)
- **Button pins**: GPIO10, 11, 12, 13, 14, 21, 47
- **RGB LED**: Onboard WS2812 on GPIO48

## Troubleshooting

### Cannot upload code
- **ESP32-WROOM**: Disconnect buttons from GPIO12, 13, 14 (strapping pins)
- Use the safe pin mapping from wiring guides

### Joystick drift
- Recalibrate with `c` command in serial monitor
- Let joystick settle at center for 2 seconds before calibrating

### RGB LED not working
- **ESP32-WROOM**: Check external LED wiring and resistors
- **ESP32-S3**: LED is onboard, no wiring needed

### Y-axis inverted
- This is handled automatically in the code
- Pushing up = move up, pulling down = squat

See detailed troubleshooting in the wiring guides.

## Development

### Building for Different Boards

The code automatically detects which board you're compiling for and selects the correct pin mappings and LED driver.

```bash
# ESP32-WROOM-32 build
pio run -e esp32doit-devkit-v1

# ESP32-S3 build
pio run -e esp32-s3-devkitc-1
```

### Adding New Board Support

1. Add new environment to `platformio.ini`
2. Add pin mappings to `src/pin_config.h` using `#ifdef`
3. Update RGB LED type if needed
4. Create wiring documentation

## Credits

- **BLE Gamepad Library**: [lemmingdev/ESP32-BLE-Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad)
- **NeoPixel Library**: [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- **KY-023 Joystick Shield**: CB Electronics

## License

This project is provided as-is for controlling the Heroman game.

---

**Made with ❤️ for Heroman** 🦸
