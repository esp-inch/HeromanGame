
// HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND


#include <Arduino.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include <Wire.h>

// Pin definitions - different for each ESP32 variant
#ifdef ESP32_S3_WROOM
  // ESP32-S3 WROOM1 pin mapping
  #define R1_PIN_DEFAULT 4
  #define G1_PIN_DEFAULT 5
  #define B1_PIN_DEFAULT 6
  #define R2_PIN_DEFAULT 7
  #define G2_PIN_DEFAULT 15
  #define B2_PIN_DEFAULT 16
  #define A_PIN_DEFAULT  18
  #define B_PIN_DEFAULT  8
  #define C_PIN_DEFAULT  3
  #define D_PIN_DEFAULT  42
  #define E_PIN_DEFAULT  21 // required for 1/32 scan panels, like 64x64
  #define LAT_PIN_DEFAULT 40
  #define OE_PIN_DEFAULT  2
  #define CLK_PIN_DEFAULT 41
#elif defined(ESP32_WROVER)
  // ESP32-WROVER pin mapping (avoids GPIO 16/17 used by PSRAM)
  #define R1_PIN_DEFAULT 25
  #define G1_PIN_DEFAULT 26
  #define B1_PIN_DEFAULT 27
  #define R2_PIN_DEFAULT 14
  #define G2_PIN_DEFAULT 12
  #define B2_PIN_DEFAULT 13
  #define A_PIN_DEFAULT 23
  #define B_PIN_DEFAULT 19
  #define C_PIN_DEFAULT 5
  #define D_PIN_DEFAULT 21 //33  // Changed from 17 (PSRAM) and 21 (I2S conflict)
  #define E_PIN_DEFAULT 32
  #define CLK_PIN_DEFAULT 22 //18  // Changed from 16 (PSRAM) and 22 (I2S conflict)
  #define LAT_PIN_DEFAULT 4
  #define OE_PIN_DEFAULT 15
#else
  // ESP32-WROOM pin mapping (original/default)
  #define R1_PIN_DEFAULT 25
  #define G1_PIN_DEFAULT 26
  #define B1_PIN_DEFAULT 27
  #define R2_PIN_DEFAULT 14
  #define G2_PIN_DEFAULT 12
  #define B2_PIN_DEFAULT 13
  #define A_PIN_DEFAULT 23
  #define B_PIN_DEFAULT 19
  #define C_PIN_DEFAULT 5
  #define D_PIN_DEFAULT 17
  #define E_PIN_DEFAULT 32 // assign to any available pin if using two panels or 64x64 panels with 1/32 scan
  #define CLK_PIN_DEFAULT 16
  #define LAT_PIN_DEFAULT 4
  #define OE_PIN_DEFAULT 15
#endif


#define R1 R1_PIN_DEFAULT
#define OE OE_PIN_DEFAULT
#define PIN_E E_PIN_DEFAULT

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 64  	// Panel height of 64 will required PIN_E to be defined.
#define PANELS_NUMBER 2 	// Number of chained panels, if just a single panel, obviously set to 1

#define PANE_WIDTH PANEL_WIDTH * PANELS_NUMBER
#define PANE_HEIGHT PANEL_HEIGHT

#define PANEL_RES_X PANEL_WIDTH    // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y PANEL_HEIGHT     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN PANELS_NUMBER      // Total number of panels chained one to another

#define RGB565(r, g, b) ((((r) & 0x1F) << 11) | (((g) & 0x3F) << 5) | ((b) & 0x1F))

// Usage:
uint16_t red = RGB565(31, 0, 0);      // Pure red
uint16_t green = RGB565(0, 63, 0);    // Pure green
uint16_t blue = RGB565(0, 0, 31);     // Pure blue
uint16_t yellow = RGB565(31, 63, 0);  // Yellow
uint16_t purple = RGB565(31, 0, 31);  // Purple
// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t time_counter = 0, cycles = 0, fps = 0;
unsigned long fps_timer;

const uint8_t kMatrixWidth = PANE_WIDTH;
const uint8_t kMatrixHeight = PANE_HEIGHT;

#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

// Include game-specific headers
#include "button.h"
#include "controller.h"
#include "heroman.h"
#include "ai_player.h"

// Global AI controllers for both players
AIController aiPlayer1 = {false, AI_BALANCED, nullptr, nullptr, 0, 0xFFFFFFFF, 0};
AIController aiPlayer2 = {false, AI_AGGRESSIVE, nullptr, nullptr, 0, 0xFFFFFFFF, 0};

void setup()
{
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize

  Serial.println("\n\n=== Heroman Starting ===");
  Serial.println("Serial initialized");

  screen_Width = PANEL_WIDTH * PANELS_NUMBER;
  screen_Height = PANEL_HEIGHT;
  Serial.println("Configuring the matrix...");

  HUB75_I2S_CFG mxconfig(
    PANEL_WIDTH,   // module width
    PANEL_HEIGHT,   // module height
    PANELS_NUMBER    // Chain length
  );

  // Explicitly set all GPIO pins
  mxconfig.gpio.r1 = R1_PIN_DEFAULT;
  mxconfig.gpio.g1 = G1_PIN_DEFAULT;
  mxconfig.gpio.b1 = B1_PIN_DEFAULT;
  mxconfig.gpio.r2 = R2_PIN_DEFAULT;
  mxconfig.gpio.g2 = G2_PIN_DEFAULT;
  mxconfig.gpio.b2 = B2_PIN_DEFAULT;
  mxconfig.gpio.a = A_PIN_DEFAULT;
  mxconfig.gpio.b = B_PIN_DEFAULT;
  mxconfig.gpio.c = C_PIN_DEFAULT;
  mxconfig.gpio.d = D_PIN_DEFAULT;
  mxconfig.gpio.e = E_PIN_DEFAULT;
  mxconfig.gpio.lat = LAT_PIN_DEFAULT;
  mxconfig.gpio.oe = OE_PIN_DEFAULT;
  mxconfig.gpio.clk = CLK_PIN_DEFAULT;

  // Double buffering tested with minimal sprite count (2 sprites) - still crashes
  // Conclusion: Double buffering is not viable due to DRAM constraints
  // Even with sprites in flash, insufficient contiguous DRAM for 32KB double buffer
  mxconfig.double_buff = true;

  #ifdef ESP32_WROVER
    // WROVER may need different clock phase due to different GPIO pins
    mxconfig.clkphase = false;
    Serial.println("WROVER: Using custom pin mapping for WROVER");
    Serial.printf("  D_PIN: %d, CLK_PIN: %d\n", D_PIN_DEFAULT, CLK_PIN_DEFAULT);
  #else
    mxconfig.clkphase = false;
  #endif

  // Try different drivers - FM6124 may not match your panels
  // Common options: SHIFTREG (generic), FM6124, FM6126A, ICN2038S, MBI5124, SM5266P
  mxconfig.driver = HUB75_I2S_CFG::FM6124;  // Most generic
  Serial.println("Using driver: FM6124");

  // OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // let's adjust default brightness to about 75%
  dma_display->setBrightness8(75);    // range is 0-255, 0 - 0%, 255 - 100%

  #ifdef ESP32_WROVER
    // WROVER may need latch blanking adjustment for different GPIO timing
    dma_display->setLatBlanking(4);
    Serial.println("WROVER: Latch blanking enabled");
  #elif defined(ESP32_S3_WROOM)
      dma_display->setLatBlanking(1);
    Serial.println("WROVER: Latch blanking enabled");
  #endif
  // Allocate memory and start DMA display
  if( not dma_display->begin() )
      Serial.println("****** !KABOOM! I2S memory allocation failed ***********");

  // Initialize hero game
  init_hero();

  // Initialize AI controllers with different personalities for interesting fights
  initAI(&aiPlayer1, &player1, &player2, AI_BALANCED);    // Player 1: Balanced fighter
  initAI(&aiPlayer2, &player2, &player1, AI_BALANCED);  // Player 2: Aggressive rushdown

  // Enable both AI for attract mode (CPU vs CPU demo)
  // Will be disabled when a real player presses start
  enableAI(&aiPlayer1);
  enableAI(&aiPlayer2);

  Serial.println("AI initialized - Attract mode enabled (CPU vs CPU demo)");
  Serial.println("Heroman initialized successfully!");
}


// Heartbeat counter for debugging

void loop(void)
{
  // Update Bluepad32 state - must be called every loop
  BP32.update();

  // Visual heartbeat - blink the white square every 50 loops (~2 seconds)
  //loopCount++;
  //if (loopCount % 50 == 0) {
  //  dma_display->fillRect(60, 0, 8, 8, (loopCount % 100 == 0) ? 0x0000 : COLOR_WHITE);
  //}

  // Connection status indicators moved to menu screen only

  // Update game state machine
  updateGameState();

  // Process input for both players (including menu for AI demo mode)
  processInput(&player1);
  processInput(&player2);

  // Check for combat collisions (only during active gameplay and menu demo)
  if (gameState == GAME_PLAYING || gameState == GAME_MENU) {
    checkCombat();
  }

  // Check for player-to-player pushback collision
  if (gameState == GAME_PLAYING || gameState == GAME_MENU) {
    checkPlayerPushback();
  }

  // Draw both players
  drawFrame();

  // 40ms delay for ~25 FPS
  delay(40);
}

