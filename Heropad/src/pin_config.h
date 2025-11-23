/*
 * Heropad - Pin Configuration
 * BLE Gamepad Controller using KY-023 Joystick Shield v1.A
 * Multi-board support with compile-time pin selection
 *
 * Supported boards:
 *   - ESP32-WROOM-32 (ESP32-DOIT-DEVKIT-V1, HiLetGo ESP32-32S)
 *   - ESP32-S3-DevKitC-1 (with onboard WS2812 RGB LED)
 *
 * Hardware: https://cb-electronics.com/products/ky-023-shield/
 */

#pragma once

// ============================================================================
// BOARD DETECTION AND PIN MAPPING
// ============================================================================

#if defined(ARDUINO_ESP32S3_DEV) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // ========================================================================
    // ESP32-S3 Pin Mapping
    // ========================================================================
    #define BOARD_ESP32_S3

    // ANALOG INPUTS - Joystick Axes (ADC1 channels for ESP32-S3)
    #define JOYSTICK_X_PIN      1       // GPIO1 (ADC1_CH0) - X-Axis potentiometer
    #define JOYSTICK_Y_PIN      2       // GPIO2 (ADC1_CH1) - Y-Axis potentiometer

    // DIGITAL INPUTS - Buttons (Active LOW with internal pull-ups)
    #define BUTTON_A_PIN        10      // GPIO10 - Button A (High punch)
    #define BUTTON_B_PIN        11      // GPIO11 - Button B (Low punch)
    #define BUTTON_C_PIN        12      // GPIO12 - Button C
    #define BUTTON_D_PIN        13      // GPIO13 - Button D
    #define BUTTON_E_PIN        14      // GPIO14 - Button E
    #define BUTTON_F_PIN        21      // GPIO21 - Button F
    #define BUTTON_K_PIN        47      // GPIO47 - Joystick button (K) - Start/Menu

    // RGB STATUS LED - Onboard WS2812 (NeoPixel)
    #define RGB_LED_PIN         48      // GPIO48 - WS2812 addressable RGB LED
    #define RGB_LED_TYPE_WS2812         // Use WS2812 driver
    #define RGB_LED_COUNT       1       // One onboard LED

#else
    // ========================================================================
    // ESP32-WROOM-32 Pin Mapping (Default)
    // ========================================================================
    #define BOARD_ESP32_WROOM

    // ANALOG INPUTS - Joystick Axes (ADC1 channels)
    #define JOYSTICK_X_PIN      34      // GPIO34 (ADC1_CH6) - X-Axis potentiometer
    #define JOYSTICK_Y_PIN      35      // GPIO35 (ADC1_CH7) - Y-Axis potentiometer

    // DIGITAL INPUTS - Buttons (Active LOW with internal pull-ups)
    #define BUTTON_A_PIN        25      // GPIO25 - Button A
    #define BUTTON_B_PIN        26      // GPIO26 - Button B
    #define BUTTON_C_PIN        27      // GPIO27 - Button C
    #define BUTTON_D_PIN        33      // GPIO33 - Button D
    #define BUTTON_E_PIN        4       // GPIO4  - Button E (SAFE - not a strapping pin!)
    #define BUTTON_F_PIN        16      // GPIO16 - Button F (SAFE - not a strapping pin!)
    #define BUTTON_K_PIN        5       // GPIO5  - Joystick button (K)

    // RGB STATUS LED - External common cathode RGB LED
    #define RGB_LED_RED_PIN     23      // GPIO23 - Red channel
    #define RGB_LED_GREEN_PIN   22      // GPIO22 - Green channel
    #define RGB_LED_BLUE_PIN    21      // GPIO21 - Blue channel
    #define RGB_LED_TYPE_GPIO           // Use simple GPIO control

#endif

// ============================================================================
// JOYSTICK CALIBRATION CONSTANTS (Common to all boards)
// ============================================================================
// ADC Resolution: 12-bit ADC (0-4095)
// Center position: ~2048
// Full left/down: ~0
// Full right/up: ~4095

#define ADC_MAX             4095    // 12-bit ADC maximum value
#define ADC_CENTER          2048    // Theoretical center position
#define DEADZONE_RADIUS     400     // Deadzone around center (± value) - increased for stability

// Thresholds for directional detection
#define AXIS_LOW_THRESHOLD  (ADC_CENTER - DEADZONE_RADIUS)   // < 1748 = LOW
#define AXIS_HIGH_THRESHOLD (ADC_CENTER + DEADZONE_RADIUS)   // > 2348 = HIGH

// ============================================================================
// PHYSICAL WIRING REFERENCE
// ============================================================================
/*
 * Pin mappings are automatically selected at compile time based on the board.
 * See JOYSTICK_WIRING.md for detailed wiring diagrams for each board variant.
 *
 * Supported boards:
 *   - ESP32-WROOM-32 (esp32doit-devkit-v1) - External RGB LED optional
 *   - ESP32-S3-DevKitC-1 (esp32-s3-devkitc-1) - Onboard WS2812 RGB LED
 *
 * IMPORTANT: Joystick shield voltage switch MUST be set to 3.3V!
 *            Connecting 5V will damage your ESP32!
 */
