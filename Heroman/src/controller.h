#pragma once

#include <stdint.h>
#include <Bluepad32.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

// Forward declarations to avoid circular dependencies
struct Player;
struct AIController;

// Button bit positions - keep same layout as before for compatibility
#define UP_BTN          0
#define DOWN_BTN        1
#define LEFT_BTN        2
#define RIGHT_BTN       3
#define SELECT_BTN      4
#define START_BTN       5
#define PUNCH_BTN       6  // A button
#define JUMP_BTN        7  // B button
#define TRI_BTN         8  // Y button
#define X_BTN           9  // X button

#define BUTTON_PRESSED(button, state) !((state >> button) & 1)
#define IS_MOVING(state) (BUTTON_PRESSED(LEFT_BTN, state) | BUTTON_PRESSED(RIGHT_BTN, state))

// RGB565 colors for visual debugging
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_WHITE   0xFFFF

// External reference to LED matrix display (defined in main.cpp)
extern MatrixPanel_I2S_DMA *dma_display;

// Forward declare AI controller struct (defined in ai_player.h)
struct AIController;

// External references to AI controllers (defined in main.cpp)
extern AIController aiPlayer1;
extern AIController aiPlayer2;

// Forward declare AI decision function
uint32_t ai_make_decision(AIController* ai);

// Status flag
bool BluePadSetupComplete = false;

// Gamepad tracking - one gamepad per player
GamepadPtr player1Gamepad = nullptr;
GamepadPtr player2Gamepad = nullptr;

// Callback when gamepad connects
void onConnectedGamepad(GamepadPtr gp) {
    Serial.printf("Gamepad connected, idx=%d\n", gp->index());

    // Visual indicator: GREEN circle at top-left = gamepad connected
    dma_display->fillCircle(10, 10, 5, COLOR_GREEN);

    // Assign to first available player slot
    if (player1Gamepad == nullptr) {
        player1Gamepad = gp;
        Serial.println("Gamepad assigned to Player 1");
        // Draw indicator for Player 1 (left side)
        dma_display->fillRect(0, 0, 10, 10, COLOR_GREEN);
    } else if (player2Gamepad == nullptr) {
        player2Gamepad = gp;
        Serial.println("Gamepad assigned to Player 2");
        // Draw indicator for Player 2 (right side)
        dma_display->fillRect(118, 0, 10, 10, COLOR_BLUE);
    } else {
        Serial.println("Warning: More than 2 gamepads connected, ignoring");
        dma_display->fillCircle(64, 10, 5, COLOR_YELLOW);
    }
}

// Callback when gamepad disconnects
void onDisconnectedGamepad(GamepadPtr gp) {
    Serial.printf("Gamepad disconnected, idx=%d\n", gp->index());

    // Visual indicator: RED circle at top-left = gamepad disconnected
    dma_display->fillCircle(10, 10, 5, COLOR_RED);

    // Clear the appropriate player gamepad reference
    if (player1Gamepad == gp) {
        player1Gamepad = nullptr;
        Serial.println("Player 1 gamepad disconnected");
        // Clear Player 1 indicator
        dma_display->fillRect(0, 0, 10, 10, COLOR_RED);
    } else if (player2Gamepad == gp) {
        player2Gamepad = nullptr;
        Serial.println("Player 2 gamepad disconnected");
        // Clear Player 2 indicator
        dma_display->fillRect(118, 0, 10, 10, COLOR_RED);
    }
}

bool isMoving(uint32_t ctrlState) {
    return (BUTTON_PRESSED(LEFT_BTN, ctrlState) || BUTTON_PRESSED(RIGHT_BTN, ctrlState));
}

void initializeController() {
    Serial.println("Initializing Bluepad32...");
    Serial.flush(); // Make sure message is sent before BP32.setup()

    // Setup Bluepad32 with connection/disconnection callbacks
    // ESP32 classic supports Classic Bluetooth (BR/EDR) - do NOT enable BLE
    // ESP32-S3 ONLY supports BLE - enable it only for S3
    #ifdef CONFIG_IDF_TARGET_ESP32S3
        BP32.enableBLEService(true);
        Serial.println("BLE service enabled (ESP32-S3 mode)");
    #else
        // ESP32 classic - use Classic Bluetooth (default, no BLE)
        Serial.println("Classic Bluetooth mode (ESP32)");
    #endif

    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BluePadSetupComplete = true;
    Serial.println("BP32.setup() completed");
    Serial.flush();

    // Disable virtual device (we want real gamepads only)
    BP32.enableVirtualDevice(false);

    // Enable new Bluetooth connections
    BP32.enableNewBluetoothConnections(true);

    Serial.println("Bluepad32 initialized - waiting for gamepad connections...");
    Serial.flush();

    // Visual indicator: Show white square at startup to confirm init
    dma_display->fillRect(60, 0, 8, 8, COLOR_WHITE);
}

// Forward declare controller functions needed by heroman.h (which is included by ai_player.h)
uint32_t controller_read_state(int playerNumber);

// Include full AI definition before function implementations
// This must come after button defines and forward declarations
#include "ai_player.h"

// Read controller state for a specific player (1 or 2)
uint32_t controller_read_state(int playerNumber) {
    // Check if AI is enabled for this player
    AIController* ai = (playerNumber == 1) ? &aiPlayer1 : &aiPlayer2;
    if (ai->enabled) {
        return ai_make_decision(ai);  // Return AI decision instead of gamepad input
    }

    // Otherwise use real gamepad input
    // Get the gamepad for this player
    GamepadPtr gp = (playerNumber == 1) ? player1Gamepad : player2Gamepad;

    // If no gamepad connected for this player, return neutral state (all buttons not pressed)
    if (gp == nullptr || !gp->isConnected()) {
        return 0xFFFFFFFF;  // All bits set = no buttons pressed
    }

    // Read D-pad state
    uint32_t u = (gp->dpad() & DPAD_UP) ? 1 : 0;
    uint32_t d = (gp->dpad() & DPAD_DOWN) ? 1 : 0;
    uint32_t l = (gp->dpad() & DPAD_LEFT) ? 1 : 0;
    uint32_t r = (gp->dpad() & DPAD_RIGHT) ? 1 : 0;

    // Read face buttons
    uint32_t a = (gp->buttons() & BUTTON_A) ? 1 : 0;      // Punch
    uint32_t b = (gp->buttons() & BUTTON_B) ? 1 : 0;      // Jump
    uint32_t x = (gp->buttons() & BUTTON_X) ? 1 : 0;
    uint32_t y = (gp->buttons() & BUTTON_Y) ? 1 : 0;      // Triangle

    // Read misc buttons for SELECT and START
    uint32_t s = (gp->miscButtons() & MISC_BUTTON_BACK) ? 1 : 0;    // Select/Back
    uint32_t t = (gp->miscButtons() & MISC_BUTTON_HOME) ? 1 : 0;    // Start/Home

    // Pack button states into bit positions - direct mapping for correct orientation
    return 0xFFFFFFFF ^ ((u << UP_BTN      /*0*/)
                      | (d << DOWN_BTN    /*1*/)
                      | (l << LEFT_BTN    /*2*/)
                      | (r << RIGHT_BTN   /*3*/)
                      | (s << SELECT_BTN  /*4*/)
                      | (t << START_BTN   /*5*/)
                      | (a << PUNCH_BTN   /*6*/)
                      | (b << JUMP_BTN    /*7*/)
                      | (x << TRI_BTN     /*8*/)
                      | (y << X_BTN       /*9*/)
                      );
}
