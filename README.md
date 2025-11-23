# HeromanGame

**Retro Side-Scrolling Fighting Game System for ESP32**

```
 ██╗  ██╗███████╗██████╗  ██████╗ ███╗   ███╗ █████╗ ███╗   ██╗
 ██║  ██║██╔════╝██╔══██╗██╔═══██╗████╗ ████║██╔══██╗████╗  ██║
 ███████║█████╗  ██████╔╝██║   ██║██╔████╔██║███████║██╔██╗ ██║
 ██╔══██║██╔══╝  ██╔══██╗██║   ██║██║╚██╔╝██║██╔══██║██║╚██╗██║
 ██║  ██║███████╗██║  ██║╚██████╔╝██║ ╚═╝ ██║██║  ██║██║ ╚████║
 ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝
```

A complete retro-style fighting game system featuring a side-scrolling beat-em-up displayed on a 64×64 RGB LED matrix, with wireless BLE gamepad controllers.

## What Is This?

HeromanGame is a **two-part system** for ESP32 microcontrollers:

1. **Heroman** - The game itself, running on an ESP32 connected to a 64×64 HUB75 LED matrix panel
2. **Heropad** - Wireless BLE gamepad controllers built with ESP32 and analog joystick shields

Fight against AI opponents or challenge a friend in two-player battles on a vibrant LED display!

---

## What You Need

### Minimum Setup (2 ESP32 Boards)

**For the Game (Heroman):**
- 1× ESP32 board (ESP32-WROOM-32, ESP32-S3, or ESP32-WROVER)
- 1× 64×64 HUB75 RGB LED Matrix Panel (P3/P4/P5 pitch)
- 1× 5V Power Supply (4-6A recommended for LED panel)

**For the Controller (Heropad) - OPTIONAL:**
- 1× ESP32 board (ESP32-WROOM-32 or ESP32-S3-DevKitC-1)
- 1× KY-023 Joystick Shield v1.A (set to 3.3V!)

**Note:** Controllers are optional! You can play using **Serial Monitor** commands if you don't have a physical controller.

### Two-Player Setup (3 ESP32 Boards)

For multiplayer battles, add a second Heropad controller (3 boards total: 1 game + 2 controllers).

---

## Project Structure

```
HeromanGame/
├── Heroman/          # The fighting game (ESP32 + LED matrix)
│   ├── src/          # Game source code
│   ├── platformio.ini
│   ├── README.md
│   └── WIRING_DIAGRAM.md
│
└── Heropad/          # BLE gamepad controller (ESP32 + joystick)
    ├── src/          # Controller source code
    ├── platformio.ini
    ├── README.md
    ├── JOYSTICK_WIRING.md
    └── JOYSTICK_WIRING_ESP32-S3.md
```

Each project has its own detailed README with wiring diagrams and build instructions.

---

## Quick Start

### Step 1: Build & Upload Heroman (The Game)

Navigate to the Heroman project and upload to your ESP32:

```bash
cd Heroman
pio run -e Esp32-S3-WROOM1 -t upload    # For ESP32-S3
# OR
pio run -e esp32dev -t upload           # For ESP32-WROOM-32
```

Wire your HUB75 LED panel following [Heroman/WIRING_DIAGRAM.md](Heroman/WIRING_DIAGRAM.md).

### Step 2: Build & Upload Heropad (Optional)

If you have a physical controller, navigate to Heropad and upload to a second ESP32:

```bash
cd Heropad
pio run -e esp32-s3-devkitc-1 -t upload    # For ESP32-S3
# OR
pio run -e esp32doit-devkit-v1 -t upload   # For ESP32-WROOM-32
```

Wire your KY-023 joystick shield following the wiring guides in `Heropad/`.

**CRITICAL:** Set the joystick shield voltage switch to **3.3V** before connecting!

### Step 3: Power On & Connect

1. Power on the Heroman game (LED panel should light up)
2. If using Heropad: Power on your controller
   - RGB LED will show **RED** (disconnected) → **GREEN** (connected)
3. If using Serial Monitor: Open serial connection at 115200 baud
   - Use `W/A/S/D` for movement, `J/K` for punches, `ENTER` to start

### Step 4: Start Playing!

- Press **START** (or send `ENTER` in serial monitor) to begin
- Fight AI opponents or battle a friend!
- Use punches, blocks, and combos to win

---

## Game Features

### Gameplay
- **Side-scrolling fighting action** - Move, punch, squat, and block
- **Rich animation system** - 28 frames per character including:
  - Running, punching, squatting, blocking
  - Hit reactions, knockdowns, victory taunts
- **Combat mechanics** - Health bars, hit detection, pushback, combos
- **AI opponents** with 4 personality types:
  - 🔥 Aggressive (rushdown)
  - 🛡️ Defensive (blocks often)
  - ⚖️ Balanced
  - 🎲 Random (beginner)
- **Two-player support** - Battle against friends via BLE gamepads

### Display
- **64×64 RGB LED Matrix** - Vivid colors and smooth animations
- **Dual-color sprites** - Red and Blue character themes
- **HUB75 interface** - Standard LED panel protocol

---

## Controller Features

### Heropad Wireless Gamepad
- **Bluetooth LE connection** - Wireless gameplay
- **Dual input modes**:
  - Physical KY-023 joystick shield (2-axis analog + 7 buttons)
  - Serial Monitor fallback (keyboard commands)
- **RGB Status LED**:
  - 🔴 Red = Disconnected
  - 🟢 Green = Connected to Heroman
  - 🔵 Blue = Calibrating
- **Multi-board support** - Works with ESP32-WROOM-32 or ESP32-S3

### Controls

**Physical Controller:**
- Joystick movement → Move / Squat
- Button A → High Punch
- Button B → Low Punch
- Button K → Start Game

**Serial Monitor (Fallback):**
- `W/A/S/D` → Move Up/Left/Down/Right
- `SPACE` → Stop moving
- `J` → High Punch
- `K` → Low Punch
- `ENTER` → Start Game
- `I` → Show joystick diagnostics
- `C` → Recalibrate joystick
- `H` → Show help

---

## How It Works

### BLE Communication

1. **Heroman** runs Bluepad32 library in gamepad server mode, waiting for connections
2. **Heropad** advertises as a BLE HID gamepad and auto-connects
3. **Standard BLE HID protocol** - D-Pad for movement, buttons for actions
4. **Auto-pairing** - First connection may require pairing; subsequent connections are automatic
5. **Multi-controller** - Up to 2 controllers can connect for two-player mode

### Architecture

```
┌─────────────────┐
│  Heropad #1     │ ──┐
│  (ESP32 + Joy)  │   │  BLE Connection
└─────────────────┘   │
                      ▼
                 ┌────────────────────┐       ┌──────────────┐
                 │   Heroman Game     │ ◄───► │  64×64 LED   │
                 │   (ESP32)          │ HUB75 │  Matrix      │
                 └────────────────────┘       └──────────────┘
                      ▲
┌─────────────────┐   │  BLE Connection
│  Heropad #2     │ ──┘
│  (ESP32 + Joy)  │ (Optional - 2-player)
└─────────────────┘
```

---

## Build Instructions

### Requirements
- **PlatformIO** (install via `pip install platformio` or VSCode extension)
- **USB cable** for programming ESP32 boards
- **Serial terminal** (built into PlatformIO or use Arduino IDE Serial Monitor)

### Building Heroman (The Game)

Navigate to the Heroman directory:

```bash
cd Heroman

# Build for ESP32-S3-WROOM1 (default)
pio run -e Esp32-S3-WROOM1 -t upload

# OR for standard ESP32-WROOM-32
pio run -e esp32dev -t upload

# OR for ESP32-WROVER
pio run -e Esp32-WROVER -t upload

# Monitor serial output
pio device monitor -b 115200
```

**Board environments:**
- `Esp32-S3-WROOM1` - ESP32-S3 (default, 921600 baud upload)
- `esp32dev` - ESP32-WROOM-32 (115200 baud upload)
- `Esp32-WROVER` - ESP32-WROVER variant

### Building Heropad (The Controller)

Navigate to the Heropad directory:

```bash
cd Heropad

# Build for ESP32-S3-DevKitC-1 (default, has onboard RGB LED)
pio run -e esp32-s3-devkitc-1 -t upload

# OR for ESP32-WROOM-32
pio run -e esp32doit-devkit-v1 -t upload

# Monitor serial output
pio device monitor -b 115200
```

**Board environments:**
- `esp32-s3-devkitc-1` - ESP32-S3 with onboard WS2812 RGB LED (default)
- `esp32doit-devkit-v1` - ESP32-WROOM-32 (requires external RGB LED)

---

## Documentation

### Heroman Game
- [Heroman/README.md](Heroman/README.md) - Project overview
- [Heroman/WIRING_DIAGRAM.md](Heroman/WIRING_DIAGRAM.md) - Complete HUB75 panel wiring with ASCII diagrams
- [Heroman/PIN_REFERENCE.txt](Heroman/PIN_REFERENCE.txt) - Quick pin mapping reference

### Heropad Controller
- [Heropad/README.md](Heropad/README.md) - Complete controller documentation
- [Heropad/JOYSTICK_WIRING.md](Heropad/JOYSTICK_WIRING.md) - ESP32-WROOM-32 wiring guide
- [Heropad/JOYSTICK_WIRING_ESP32-S3.md](Heropad/JOYSTICK_WIRING_ESP32-S3.md) - ESP32-S3 wiring guide
- [Heropad/HEROPAD_SCHEMATIC_ESP32-S3.md](Heropad/HEROPAD_SCHEMATIC_ESP32-S3.md) - Breadboard schematic

---

## Technology Stack

### Heroman Game
- **Framework:** Arduino (via PlatformIO)
- **Display Library:** ESP32 HUB75 LED Matrix Panel DMA Display
- **Graphics:** Adafruit GFX Library
- **Gamepad Support:** Bluepad32 (custom Arduino package)
- **Build System:** PlatformIO

### Heropad Controller
- **Framework:** Arduino (via PlatformIO)
- **BLE Library:** ESP32-BLE-Gamepad
- **RGB LED:** Adafruit NeoPixel (ESP32-S3 only)
- **Build System:** PlatformIO

---

## Troubleshooting

### Heroman Game Issues

**LED Panel not lighting up:**
- Verify 5V power supply is connected to panel (separate from ESP32 USB power)
- Check all HUB75 cable connections (especially GND!)
- Confirm pin mappings match your ESP32 variant (see WIRING_DIAGRAM.md)

**Gamepad not connecting:**
- Ensure Heropad is powered on and shows RED LED (searching for connection)
- Check serial monitor for "Waiting for gamepad..." message
- Try power-cycling the Heropad controller
- Use Serial Monitor commands as fallback

**Game crashes or reboots:**
- LED panels draw high current - ensure adequate 5V power supply (4-6A)
- Check for shorts in HUB75 wiring
- Verify PSRAM settings in platformio.ini match your board

### Heropad Controller Issues

**Cannot upload code / Boot loop:**
- On ESP32-WROOM-32: Avoid GPIO12, 13, 14 (strapping pins)
- Use the safe pin mappings in the wiring guides
- Disconnect buttons during upload if using GPIO12-14

**Joystick drift:**
- Recalibrate with `C` command in serial monitor
- Let joystick settle at center for 2 seconds before calibrating
- Check voltage switch is set to 3.3V (NOT 5V!)

**RGB LED not working:**
- ESP32-S3: LED is onboard (GPIO48), no wiring needed
- ESP32-WROOM-32: Requires external RGB LED with 220Ω resistors

**Buttons not responding:**
- Verify internal pull-ups are enabled (code does this automatically)
- Check buttons aren't shorted to GND
- GPIO32 has weak pull-ups - use GPIO5 instead (see wiring guides)

For detailed troubleshooting, see the individual project README files.

---

## Credits

### Libraries Used
- [ESP32 HUB75 LED Matrix Panel DMA Display](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA) by mrfaptastic
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Bluepad32](https://gitlab.com/ricardoquesada/bluepad32) by Ricardo Quesada
- [ESP32-BLE-Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad) by lemmingDev
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)

### Hardware
- **ESP32** microcontroller platform by Espressif Systems
- **HUB75 LED panels** - Standard addressable RGB LED matrix protocol
- **KY-023 Joystick Shield** by CB Electronics

---

## License

This project is provided as-is for educational and entertainment purposes.

---

**Made with ❤️ for retro gaming enthusiasts**
