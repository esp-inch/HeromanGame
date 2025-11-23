# Heroman - ESP32 Wiring Guide

## Hardware Required
- **ESP32 DevKit** (standard 30-pin version, NOT ESP32-S3 or WROVER)
- **64x64 RGB LED Matrix Panel** (HUB75 interface, 1/32 scan)
- **5V Power Supply** (4-6A recommended for full brightness)
- **Bluetooth Gamepads** (PS4, Xbox, 8BitDo, or similar)

---

## HUB75 LED Matrix Panel Pinout

### ESP32 to HUB75 Connection Table

| HUB75 Pin | Signal | ESP32 GPIO | Description |
|-----------|--------|------------|-------------|
| R1 | Red (Upper) | GPIO 25 | Upper half red data |
| G1 | Green (Upper) | GPIO 26 | Upper half green data |
| B1 | Blue (Upper) | GPIO 27 | Upper half blue data |
| R2 | Red (Lower) | GPIO 14 | Lower half red data |
| G2 | Green (Lower) | GPIO 12 | Lower half green data |
| B2 | Blue (Lower) | GPIO 13 | Lower half blue data |
| A | Row Select A | GPIO 23 | Row address bit 0 |
| B | Row Select B | GPIO 19 | Row address bit 1 |
| C | Row Select C | GPIO 5 | Row address bit 2 |
| D | Row Select D | GPIO 17 | Row address bit 3 |
| E | Row Select E | GPIO 32 | Row address bit 4 (for 1/32 scan) |
| CLK | Clock | GPIO 16 | Shift register clock |
| LAT | Latch | GPIO 4 | Data latch |
| OE | Output Enable | GPIO 15 | Brightness control (active low) |
| GND | Ground | GND | Ground connection |

### Power Connections

⚠️ **IMPORTANT**: The ESP32 and LED panel must share a **common ground**!

| Component | Connection | Notes |
|-----------|------------|-------|
| ESP32 | USB or VIN (5V) | Powers the microcontroller |
| LED Panel | 5V Power Input | **Separate 5V supply (4-6A)** |
| GND | **Common Ground** | Connect ESP32 GND to Panel GND |

---

## ASCII Wiring Diagram

```
┌─────────────────────────────────────────┐
│   5V Power Supply (4-6A)                │
│   ┌─────────────────────────────────┐   │
│   │  +5V                        GND │   │
│   └───┬─────────────────────────┬───┘   │
│       │                         │       │
└───────┼─────────────────────────┼───────┘
        │                         │
        ▼                         ▼
    ┌───────────────────────────────────┐
    │     64x64 HUB75 LED Panel         │
    │  ┌─────────────────────────────┐  │
    │  │ R1  G1  B1  GND             │  │◄── Data Input Connector
    │  │ R2  G2  B2  E               │  │
    │  │  A   B   C   D              │  │
    │  │CLK LAT  OE  GND             │  │
    │  └──┬──┬──┬──┬──┬──┬──┬──┬──┬─┘  │
    └─────┼──┼──┼──┼──┼──┼──┼──┼──┼────┘
          │  │  │  │  │  │  │  │  │
     ┌────┼──┼──┼──┼──┼──┼──┼──┼──┼────────────────┐
     │    │  │  │  │  │  │  │  │  └─GND            │
     │    │  │  │  │  │  │  │  └─OE (15)           │
     │ ┌──┼──┼──┼──┼──┼──┼──┼─LAT (4)              │
     │ │  │  │  │  │  │  │  └─CLK (16)             │
     │ │  │  │  │  │  │  └─────D (17)              │
     │ │  │  │  │  │  └────────C (5)               │
     │ │  │  │  │  └───────────B (19)              │
     │ │  │  │  └──────────────A (23)              │
     │ │  │  └────────────────E (32)               │
     │ │  └──────────────────B2 (13)               │
     │ └────────────────────G2 (12)                │
     │ ┌───────────────────R2 (14)                 │
     │ │  ┌────────────────B1 (27)                 │
     │ │  │  ┌─────────────G1 (26)                 │
     │ │  │  │  ┌──────────R1 (25)                 │
     │ │  │  │  │                                   │
     │ ▼  ▼  ▼  ▼                                   │
     │ ┌────────────────────────────────┐           │
     │ │       ESP32 DevKit             │           │
     │ │                                │           │
     │ │  [25][26][27]    [14][12][13] │           │
     │ │  [23][19][ 5][17][32]          │           │
     │ │  [16][ 4][15]                  │           │
     │ │                                │           │
     │ │  [GND]───────────┬─────────────┼───────────┤
     │ │                  │             │           │
     │ │  [USB/VIN 5V]    │             │           │
     │ └──────────────────┼─────────────┘           │
     │                    │                         │
     └────────────────────┼─────────────────────────┘
                          │
                       Common
                       Ground
```

---

## Simplified Breadboard Layout

For prototyping, you can use jumper wires:

```
ESP32          HUB75 Panel
=====          ===========
GPIO 25  ────► R1
GPIO 26  ────► G1
GPIO 27  ────► B1
GPIO 14  ────► R2
GPIO 12  ────► G2
GPIO 13  ────► B2
GPIO 23  ────► A
GPIO 19  ────► B
GPIO  5  ────► C
GPIO 17  ────► D
GPIO 32  ────► E
GPIO 16  ────► CLK
GPIO  4  ────► LAT
GPIO 15  ────► OE
GND      ────► GND (MUST be connected!)
```

---

## Controller Setup (Bluetooth)

**No wiring required!** The game uses Bluepad32 for wireless Bluetooth gamepad support.

### Supported Controllers
- PlayStation 4/5 Controllers
- Xbox One/Series Controllers
- Nintendo Switch Pro Controllers
- 8BitDo Controllers
- Generic Bluetooth gamepads

### Pairing Instructions
1. Upload code to ESP32
2. Power on ESP32 (Bluetooth auto-enables)
3. Put controller in pairing mode:
   - **PS4**: Hold SHARE + PS button until light flashes
   - **Xbox**: Hold pairing button on top until LED flashes
   - **8BitDo**: Hold START + button for ~3s
4. Controller should auto-connect (LED panel shows indicators)

### Visual Indicators on LED Panel
- **Top-left GREEN square**: Player 1 connected
- **Top-right BLUE square**: Player 2 connected
- **RED circle**: Controller disconnected

---

## Power Supply Sizing

### Current Draw Estimates
| Brightness | Typical Current | Recommended PSU |
|------------|----------------|-----------------|
| 25% (dim) | 1-2A @ 5V | 3A minimum |
| 50% (medium) | 2-3A @ 5V | 4A recommended |
| 100% (full) | 4-5A @ 5V | 6A recommended |

**Recommendation**: Use a 5V 5A power supply for reliable operation.

---

## Troubleshooting

### Panel doesn't light up
- ✓ Check 5V power supply is connected to panel
- ✓ Verify common ground between ESP32 and panel
- ✓ Double-check all 15 data pins are connected correctly

### Flickering or wrong colors
- ✓ Ensure CLK (GPIO 16) and LAT (GPIO 4) are correct
- ✓ Try shorter jumper wires (reduce signal noise)
- ✓ Add 100μF capacitor across panel 5V/GND

### Controllers won't pair
- ✓ Ensure Bluepad32 library is installed
- ✓ Check Serial Monitor for pairing messages
- ✓ Verify controller is in pairing mode (flashing LED)
- ✓ Try resetting controller (small button on back)

---

## Bill of Materials (BOM)

| Item | Qty | Approx. Cost | Notes |
|------|-----|--------------|-------|
| ESP32 DevKit | 1 | $5-10 | 30-pin standard version |
| 64x64 RGB LED Panel | 1 | $20-35 | HUB75, P3/P4/P5 pitch |
| 5V 5A Power Supply | 1 | $8-15 | Barrel jack or screw terminal |
| Jumper Wires (F-F) | 20 | $3-5 | Or use ribbon cable |
| Bluetooth Gamepad | 1-2 | $15-60 | Optional (AI can play) |

**Total Cost**: ~$50-120 depending on components chosen

---

## Notes for Video Tutorial

### Scene Suggestions
1. **Parts Overview** (5s): Show all components laid out
2. **Pin Reference** (10s): Show HUB75 connector closeup with labels
3. **Wiring Process** (30s): Time-lapse of connecting wires
4. **Power Connection** (10s): Highlight common ground importance
5. **Upload & Test** (15s): Show serial monitor + panel boot sequence
6. **Controller Pairing** (15s): Demonstrate pairing process
7. **Gameplay** (30s): Show final game running

### Text Overlays
- "⚠️ IMPORTANT: Connect common ground!"
- "Use GPIO pins shown in table →"
- "5V 5A power supply recommended"
- "No wiring needed for controllers!"
