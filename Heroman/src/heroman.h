
#pragma once

//#include "pin_config.h"
//#include <TFT_eSPI.h> // Hardware-specific library
#include "sprites.h"
#include "images.h"
#include "player.h"
#include <math.h>
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
// Set delay after plotting the sprite
#define DELAY 500
#define MAX_SPEED 3  // Reduced from 7 for smoother movement
#define ANIMATION_FRAME_DELAY 2  // Update animation every N frames for smoother look

#define ANIMATION_STOPPED 0
#define ANIMATION_RUNNING 1
#define ANIMATION_PUNCHING_HIGH 2
#define ANIMATION_PUNCHING_LOW 3
//#define ANIMATION_JUMPPUNCHING_HIGH 4
//#define ANIMATION_JUMPPUNCHING_LOW 5
#define ANIMATION_SQUATTING 6
#define ANIMATION_SQUATPUNCHING_HIGH 7
#define ANIMATION_SQUATPUNCHING_LOW 8
#define ANIMATION_BLOCKING 9
//#define ANIMATION_JUMPBLOCKING 10
#define ANIMATION_SQUATBLOCKING 11
#define ANIMATION_STAGGER 12  // High hit reaction (single frame)
#define ANIMATION_STAGGER_LOW 13  // Low hit reaction (single frame)
#define ANIMATION_KNOCKDOWN 14  // Knockdown sequence
#define ANIMATION_DYING 15  // Dying animation (frames 8, 14)
#define ANIMATION_DEAD 16  // Death state (frame 15)

// Game states
#define GAME_MENU 0  // Start menu
#define GAME_COUNTDOWN 1  // "Ready! Set! Fight!" countdown
#define GAME_PLAYING 2
#define GAME_FREEZE_FRAME 3  // Brief pause when killing blow lands
#define GAME_VICTORY_PAUSE 4  // Pause before winner starts walking
#define GAME_VICTORY_WALK 5  // Winner walks to loser
#define GAME_VICTORY_TAUNT 6  // Winner does 4 squats
#define GAME_RESET_COUNTDOWN 7  // Freeze before reset

// Direction enum moved to player.h

int player_animation_index_stopped[]{0};
int player_animation_index_running[]{ 20, 21, 22, 23, 24, 25, 26, 27 };
int player_animation_index_punching_high[]{ 3, 3 };  // Punch frame duplicated - more visible!
int player_animation_index_punching_low[]{ 4, 4 };   // Punch frame duplicated - more visible!
//int player_animation_index_jumppunching_high[]{ 11 };  // Row 2, image 4: jump high punch
//int player_animation_index_jumppunching_low[]{ 12 };   // Row 2, image 5: jump low punch
int player_animation_index_squatting[]{1};
int player_animation_index_squatpunching_high[]{ 17, 17 };  // Punch frame duplicated - more visible!
int player_animation_index_squatpunching_low[]{ 18, 18 };   // Punch frame duplicated - more visible!
int player_animation_index_blocking[]{ 2 };  // Row 1, image 3: standing block
//int player_animation_index_jumpblocking[]{ 10 };  // Row 2, image 3: jump block
int player_animation_index_squatblocking[]{ 16 };  // Row 3, image 3: squat block
int player_animation_index_stagger[]{ 8 };  // Image 8: high hit flinch
int player_animation_index_stagger_low[]{ 7 };  // Image 7: low hit flinch
int player_animation_index_knockdown[]{ 14, 15 };  // Row 3, frames 0-1: knockdown sequence
int player_animation_index_dying[]{ 8, 14, 15 };  // Dying sequence: flinch (8) then fall (14)
int player_animation_index_dead[]{ 15 };  // Dead on ground

//int playey_animation_frame_data[]{}

// Screen dimensions (global)
int screen_Width = 0;
int screen_Height = 0;

// Game state
int gameState = GAME_MENU;  // Start at menu
Player* winner = nullptr;
Player* loser = nullptr;
int stateTimer = 0;  // General purpose timer for various game states
int victorySquatCount = 0;  // Count squats during victory taunt
bool winnerStartedOnLeft = false;  // Track which side winner started on for victory walk
int menuBlinkTimer = 0;  // For blinking "PRESS A" text

// Timing constants (in frames at 25 FPS)
const int FREEZE_FRAME_DELAY = 30;  // ~1.2 seconds freeze on killing blow
const int VICTORY_PAUSE_DELAY = 30;  // ~1.2 seconds pause before walk
const int VICTORY_WALK_SPEED = 2;  // Slow walk speed
const int SQUAT_HOLD_FRAMES = 15;  // ~0.6 seconds per squat
const int RESET_FREEZE_DELAY = 50;  // ~2 seconds freeze before reset

// Player instances - replaces all previous global player state variables
// IMPORTANT: Add padding between players to prevent DMA buffer overflow corruption
Player player1;
uint8_t memoryGuard[8192];  // 8KB safety buffer to catch DMA overflows
Player player2;

// Safe position setter with validation
void setPlayerXPos(Player* p, int newXPos, const char* source) {
  // Validate position
  if (newXPos < -100 || newXPos > 200) {
    return; // Don't allow invalid positions
  }

  p->xPos = newXPos;
}

// Flip a sprite horizontally by reversing pixels in each row
// Now uses player's own flip buffer
const uint16_t* flipSpriteHorizontal(Player* p, const uint16_t* source, int width, int height) {
  // Validate dimensions to prevent buffer overflow
  if (width != SPRITE_FRAME_WIDTH || height != SPRITE_FRAME_HEIGHT) {
    Serial.printf("ERROR: flipSpriteHorizontal called with invalid dimensions: %dx%d (expected %dx%d)\n",
                  width, height, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT);
    return source; // Return original unflipped sprite to avoid corruption
  }

  //Serial.printf("Player %d flipping sprite (direction=%d)\n", p->playerNumber, p->direction);

  for (int row = 0; row < height; row++) {
    int rowStart = row * width;
    for (int col = 0; col < width; col++) {
      int destIndex = rowStart + col;
      int sourceIndex = rowStart + (width - 1 - col);

      // Bounds check
      if (destIndex >= SPRITE_FRAME_WIDTH * SPRITE_FRAME_HEIGHT) {
        Serial.printf("OVERFLOW DETECTED in Player %d flip! destIndex=%d exceeds buffer size %d\n",
                      p->playerNumber, destIndex, SPRITE_FRAME_WIDTH * SPRITE_FRAME_HEIGHT);
        return source;
      }

      // Reverse the column order within each row
      p->flippedSpriteBuffer[destIndex] = source[sourceIndex];
    }
  }

  //Serial.printf("Player %d flip complete\n", p->playerNumber);
  return p->flippedSpriteBuffer;
}

#include "controller.h"


int calcNextPos(Player* p){
	int result = p->xPos;

  if(BUTTON_PRESSED(LEFT_BTN, p->ctrlState)) {
    result = p->xPos - p->speed;
    // Clamp to left edge instead of wrapping
    if (result < 0) {
      result = 0;
      //Serial.printf("Player %d clamped to left edge\n", p->playerNumber);
    }
  }

  if(BUTTON_PRESSED(RIGHT_BTN, p->ctrlState)) {
    result = p->xPos + p->speed;
    // Clamp to right edge instead of wrapping
    int maxPos = screen_Width - SPRITE_FRAME_WIDTH;
    if (result > maxPos) {
      result = maxPos;
      //Serial.printf("Player %d clamped to right edge\n", p->playerNumber);
    }
  }

	return result;
}

bool isMidAnimation(Player* p){
	bool result = false;
  int frameCount = ARRAYSIZE(p->animationFrameset);

	switch(p->animation){
		case ANIMATION_PUNCHING_HIGH:
		case ANIMATION_PUNCHING_LOW:
		//case ANIMATION_JUMPPUNCHING_HIGH:
		//case ANIMATION_JUMPPUNCHING_LOW:
		case ANIMATION_SQUATPUNCHING_HIGH:
		case ANIMATION_SQUATPUNCHING_LOW:
		case ANIMATION_BLOCKING:
		//case ANIMATION_JUMPBLOCKING:
		case ANIMATION_SQUATBLOCKING:
			result = (p->animationFrameIndex < frameCount-1) ? true : false;
		break;
	}

	return result;
}


int calcNextAnimationIndex(Player* p){
	int result = 0;
  int frameCount = 0;
	switch(p->animation){
		case ANIMATION_STOPPED:
      result = 0;
      break;
		case ANIMATION_SQUATTING:
      result = 0;
      break;
		case ANIMATION_RUNNING:
      frameCount = ARRAYSIZE(player_animation_index_running);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_PUNCHING_HIGH:
      frameCount = ARRAYSIZE(player_animation_index_punching_high);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_PUNCHING_LOW:
      frameCount = ARRAYSIZE(player_animation_index_punching_low);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		//case ANIMATION_JUMPPUNCHING_HIGH:
    //  frameCount = ARRAYSIZE(player_animation_index_jumppunching_high);
		//	result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		//break;
		//case ANIMATION_JUMPPUNCHING_LOW:
    //  frameCount = ARRAYSIZE(player_animation_index_jumppunching_low);
		//	result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		//break;
		case ANIMATION_SQUATPUNCHING_HIGH:
      frameCount = ARRAYSIZE(player_animation_index_squatpunching_high);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_SQUATPUNCHING_LOW:
      frameCount = ARRAYSIZE(player_animation_index_squatpunching_low);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_BLOCKING:
      frameCount = ARRAYSIZE(player_animation_index_blocking);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		//case ANIMATION_JUMPBLOCKING:
    //  frameCount = ARRAYSIZE(player_animation_index_jumpblocking);
		//	result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		//break;
		case ANIMATION_SQUATBLOCKING:
      frameCount = ARRAYSIZE(player_animation_index_squatblocking);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_STAGGER:
      frameCount = ARRAYSIZE(player_animation_index_stagger);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_STAGGER_LOW:
      frameCount = ARRAYSIZE(player_animation_index_stagger_low);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_KNOCKDOWN:
      frameCount = ARRAYSIZE(player_animation_index_knockdown);
			result = (p->animationFrameIndex >= frameCount-1) ? 0 : p->animationFrameIndex + 1;
		break;
		case ANIMATION_DYING:
      frameCount = ARRAYSIZE(player_animation_index_dying);
			// Don't loop - stay on last frame when dying animation completes
			result = (p->animationFrameIndex >= frameCount-1) ? (frameCount-1) : p->animationFrameIndex + 1;
		break;
		case ANIMATION_DEAD:
      // Dead - stay on frame 0 forever
			result = 0;
		break;
    default:
		break;
	}
	return result;
}



void setAnimation(Player* p, int newAnimation, int* newFrameset)
{
  if(p->animation != newAnimation)
  {
    //Serial.printf("Changing Animation=%d\n", newAnimation);
    p->animation = newAnimation;
    p->animationFrameIndex = 0;
    p->animationFrameset = newFrameset;
    p->animationDelayCounter = 0;  // Reset delay counter on animation change
    //Serial.printf("First frame index=%d\n", p->animationFrameset[0]);
  }
  else
  {
    // Increment delay counter
    p->animationDelayCounter++;

    // Only advance animation frame when delay counter reaches threshold
    if (p->animationDelayCounter >= ANIMATION_FRAME_DELAY) {
      p->animationFrameIndex = calcNextAnimationIndex(p);
      p->animationDelayCounter = 0;  // Reset counter
    }
  }

}

void setAnimationFromControlState(Player* p)
{
  // Check for blocking first (TRI_BTN = Y button)
  if(BUTTON_PRESSED(TRI_BTN, p->ctrlState))
  {
    if(BUTTON_PRESSED(DOWN_BTN, p->ctrlState))
    {
      setAnimation(p, ANIMATION_SQUATBLOCKING, player_animation_index_squatblocking);
    }
    else if(isMoving(p->ctrlState))
    {
      //setAnimation(p, ANIMATION_JUMPBLOCKING, player_animation_index_jumpblocking);
    }
    else
    {
      setAnimation(p, ANIMATION_BLOCKING, player_animation_index_blocking);
    }
  }
  // Check for high punch (PUNCH_BTN = A button)
  else if(BUTTON_PRESSED(PUNCH_BTN, p->ctrlState) && p->canPunch)
  {
    // Set punch cooldown to prevent spamming
    p->canPunch = false;
    p->punchCooldown = 10; // 20 frames (~0.8 seconds) between punches

    if(BUTTON_PRESSED(DOWN_BTN, p->ctrlState))
    {
      setAnimation(p, ANIMATION_SQUATPUNCHING_HIGH, player_animation_index_squatpunching_high);
    }
    else if(isMoving(p->ctrlState))
    {
      //setAnimation(p, ANIMATION_JUMPPUNCHING_HIGH, player_animation_index_jumppunching_high);
    }
    else
    {
      setAnimation(p, ANIMATION_PUNCHING_HIGH, player_animation_index_punching_high);
    }
  }
  // Check for low punch (JUMP_BTN = B button, repurposed as low punch)
  else if(BUTTON_PRESSED(JUMP_BTN, p->ctrlState) && p->canPunch)
  {
    // Set punch cooldown to prevent spamming
    p->canPunch = false;
    p->punchCooldown = 10; // 20 frames (~0.8 seconds) between punches

    if(BUTTON_PRESSED(DOWN_BTN, p->ctrlState))
    {
      setAnimation(p, ANIMATION_SQUATPUNCHING_LOW, player_animation_index_squatpunching_low);
    }
    else if(isMoving(p->ctrlState))
    {
      //setAnimation(p, ANIMATION_JUMPPUNCHING_LOW, player_animation_index_jumppunching_low);
    }
    else
    {
      setAnimation(p, ANIMATION_PUNCHING_LOW, player_animation_index_punching_low);
    }
  }
  else if(isMoving(p->ctrlState))
  {
    setAnimation(p, ANIMATION_RUNNING, player_animation_index_running);
  }
  else
  {
    if(BUTTON_PRESSED(DOWN_BTN, p->ctrlState))
    {
      setAnimation(p, ANIMATION_SQUATTING, player_animation_index_squatting);
    }
    else
    {
      setAnimation(p, ANIMATION_STOPPED, player_animation_index_stopped);
    }
  }
}


// No longer needed - we now flip sprites at runtime
// const unsigned short ** getCurrentDirectionalFrameset()
// {
//   return (lastKnown_Direction == MovingLeft) ? player_left_frames : player_right_frames;
// }

// Check if two players are colliding (bounding box collision)
bool checkCollision(Player* p1, Player* p2)
{
  // Define hitboxes - account for ~8px blank space on each side of sprite
  // Sprite is 46x50 but actual character occupies ~30x45 in center
  int hitboxWidth = SPRITE_FRAME_WIDTH - 16;   // 30px (8px blank on each side)
  int hitboxHeight = SPRITE_FRAME_HEIGHT - 5;  // 45px
  int hitboxOffsetX = 8;  // Skip 8px of left-side blank space
  int hitboxOffsetY = 0;

  // Player 1 hitbox
  int p1Left = p1->xPos + hitboxOffsetX;
  int p1Right = p1->xPos + hitboxOffsetX + hitboxWidth;
  int p1Top = p1->yPos + hitboxOffsetY;
  int p1Bottom = p1->yPos + hitboxOffsetY + hitboxHeight;

  // Player 2 hitbox
  int p2Left = p2->xPos + hitboxOffsetX;
  int p2Right = p2->xPos + hitboxOffsetX + hitboxWidth;
  int p2Top = p2->yPos + hitboxOffsetY;
  int p2Bottom = p2->yPos + hitboxOffsetY + hitboxHeight;

  // Check for overlap
  return !(p1Right < p2Left || p1Left > p2Right || p1Bottom < p2Top || p1Top > p2Bottom);
}

// Handle when a player gets hit
void applyHit(Player* victim, Player* attacker, bool isLowPunch)
{
  // Check if victim is already in hit stagger or dead/dying
  if (victim->isHit || victim->isDead) {
    return; // Already in hit stagger or dead
  }

  // Apply damage
  victim->health -= 10;
  if (victim->health < 0) victim->health = 0;

  // Check if this hit killed the player
  if (victim->health <= 0) {
    // Player is dead - start dying animation
    victim->isDead = true;
    setAnimation(victim, ANIMATION_DYING, player_animation_index_dying);

    // Turn victim to face attacker
    if (attacker->xPos < victim->xPos) {
      victim->direction = MovingLeft;  // Attacker is on the left
    } else {
      victim->direction = MovingRight; // Attacker is on the right
    }

    // Trigger freeze frame and victory sequence
    gameState = GAME_FREEZE_FRAME;
    winner = attacker;
    loser = victim;
    stateTimer = FREEZE_FRAME_DELAY;

    Serial.printf("GAME OVER! Player %d wins!\n", attacker->playerNumber);

    return; // Don't do stagger or pushback for death
  }

  // Set hit state with appropriate stagger animation
  victim->isHit = true;
  victim->hitCooldown = 15; // 15 frames of stagger (~0.6 seconds at 25 FPS)

  // Set the appropriate stagger animation with correct frameset
  if (isLowPunch) {
    setAnimation(victim, ANIMATION_STAGGER_LOW, player_animation_index_stagger_low);
  } else {
    setAnimation(victim, ANIMATION_STAGGER, player_animation_index_stagger);
  }

  // Turn victim to face attacker
  if (attacker->xPos < victim->xPos) {
    victim->direction = MovingLeft;  // Attacker is on the left
  } else {
    victim->direction = MovingRight; // Attacker is on the right
  }

  // Push back slightly
  int newPos;
  if (attacker->xPos < victim->xPos) {
    newPos = victim->xPos + 3; // Push right
  } else {
    newPos = victim->xPos - 3; // Push left
  }

  // Clamp to screen bounds
  if (newPos < 0) {
    newPos = 0;
  }
  if (newPos > screen_Width - SPRITE_FRAME_WIDTH) {
    newPos = screen_Width - SPRITE_FRAME_WIDTH;
  }

  setPlayerXPos(victim, newPos, "applyHit");
}


void processInput(Player* p)
{
  // Debug: Log processInput being called during menu
  static int processInputDebugCounter = 0;
  if (gameState == GAME_MENU && processInputDebugCounter++ % 60 == 0) {
    Serial.printf("processInput P%d: gameState=%d, ctrlState=0x%08X\n",
                 p->playerNumber, gameState, p->ctrlState);
  }

  // During game over sequence, only process dying animation for loser
  if (gameState != GAME_PLAYING && gameState != GAME_MENU && gameState != GAME_COUNTDOWN && p != loser) {
    return; // Don't process input for winner during victory sequence
  }

  // During countdown, don't process any input - players should stand still
  if (gameState == GAME_COUNTDOWN) {
    return;
  }

  // Read controller state for this player's gamepad (during gameplay and menu attract mode)
  if (gameState == GAME_PLAYING || gameState == GAME_MENU) {
    p->ctrlState = controller_read_state(p->playerNumber);
  }

  // If player is dying or dead, handle death animation
  if (p->isDead) {
    if (p->animation == ANIMATION_DYING) {
      // Advance dying animation
      int frameCount = ARRAYSIZE(player_animation_index_dying);
      int nextFrame = calcNextAnimationIndex(p);

      // Only move backwards during FREEZE_FRAME state, then freeze in place
      if (gameState == GAME_FREEZE_FRAME) {
        // Move backwards during dying animation (away from attacker)
        // Direction was set to face attacker, so move in that direction (backwards)
        int dyingSpeed = 5;  // Slower than normal movement
        int newPos = p->xPos;

        if (p->direction == MovingLeft) {
          newPos += dyingSpeed;  // Move left (backwards from right attacker)
        } else {
          newPos -= dyingSpeed;  // Move right (backwards from left attacker)
        }

        // Clamp to screen bounds
        if (newPos < 0) newPos = 0;
        if (newPos > screen_Width - SPRITE_FRAME_WIDTH) {
          newPos = screen_Width - SPRITE_FRAME_WIDTH;
        }
        setPlayerXPos(p, newPos, "dying");
      }

      // Check if dying animation completed
      if (p->animationFrameIndex >= frameCount - 1 && nextFrame == frameCount - 1) {
        // Dying animation complete - transition to dead state
        setAnimation(p, ANIMATION_DEAD, player_animation_index_dead);
      } else {
        p->animationFrameIndex = nextFrame;
      }
    }
    // Dead or dying - no input processing
    p->speed = 0;
    return;
  }

  // Handle cooldowns
  if (p->hitCooldown > 0) {
    p->hitCooldown--;
    if (p->hitCooldown == 0) {
      p->isHit = false; // Recovery complete
    }
  }

  if (p->punchCooldown > 0) {
    p->punchCooldown--;
    if (p->punchCooldown == 0) {
      p->canPunch = true;
    }
  }

  if (p->pushbackCooldown > 0) {
    p->pushbackCooldown--;
  }

  // If player is in hit stagger, override all input
  if (p->isHit) {
    // Advance the current stagger animation frame (don't override - applyHit already set it)
    p->animationFrameIndex = calcNextAnimationIndex(p);
    p->speed = 0;
    return; // Don't process other input while hit
  }

  if(isMidAnimation(p))
  {
    p->animationFrameIndex = calcNextAnimationIndex(p);
  }
  else
  {
    if(isMoving(p->ctrlState))
    {
      // Update direction based on which direction button is pressed
      p->direction = BUTTON_PRESSED(LEFT_BTN, p->ctrlState) ? MovingLeft : p->direction;
      p->direction = BUTTON_PRESSED(RIGHT_BTN, p->ctrlState) ? MovingRight : p->direction;
      p->speed = MAX_SPEED;
      int newPos = calcNextPos(p);
      setPlayerXPos(p, newPos, "processInput-movement");
    }
    else
    {
      p->speed = 0;
    }

    setAnimationFromControlState(p);

  }

}


// Transparent color key - pure black (0x0000) will be treated as transparent
#define TRANSPARENT_COLOR 0x0000

// Generate a transparency mask from sprite data
void generateSpriteMask(const uint16_t* spriteData, uint8_t* maskBuffer) {
  // Clear mask buffer - size must match Player struct allocation (6 bytes/row * 50 rows = 300 bytes)
  int bytesPerRow = (SPRITE_FRAME_WIDTH + 7) / 8;
  int maskBufferSize = bytesPerRow * SPRITE_FRAME_HEIGHT;
  memset(maskBuffer, 0, maskBufferSize);

  // Generate mask: set bit to 1 if pixel is NOT transparent
  // Mask uses MSB-first bit order within each byte
  for (int y = 0; y < SPRITE_FRAME_HEIGHT; y++) {
    for (int x = 0; x < SPRITE_FRAME_WIDTH; x++) {
      int pixelIndex = y * SPRITE_FRAME_WIDTH + x;

      if (spriteData[pixelIndex] != TRANSPARENT_COLOR) {
        // Calculate mask position: MSB first (bit 7 = pixel 0, bit 6 = pixel 1, etc.)
        int byteIndex = y * bytesPerRow + (x / 8);
        int bitPosition = 7 - (x % 8);  // MSB to LSB
        maskBuffer[byteIndex] |= (1 << bitPosition);
      }
    }
  }
}

// Draw a single player with transparency support using hardware-accelerated masking
void drawPlayer(Player* p)
{
  // Validate position before drawing
  if (p->xPos < -100 || p->xPos > 200) {
    p->xPos = (p->playerNumber == 1) ? 10 : (screen_Width - 60);
  }

  // Use player's sprite set
  const unsigned short ** frames = p->spriteFrames;
  int imageIndex = p->animationFrameset[p->animationFrameIndex];

  // Get the sprite data
  const uint16_t* spriteData = frames[imageIndex];

  // Flip horizontally if facing left
  if (p->direction == MovingLeft) {
    //Serial.printf("drawPlayer: Player %d needs flip (direction=MovingLeft)\n", p->playerNumber);
    spriteData = flipSpriteHorizontal(p, spriteData, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT);
  }

  // Generate transparency mask from sprite data into player's own buffer
  generateSpriteMask(spriteData, p->maskBuffer);

  // Draw with mask using optimized library function
  int yPos = screen_Height - STONEWALL_HEIGHT - 50 + 2;

  dma_display->drawRGBBitmap(p->xPos, yPos, spriteData, p->maskBuffer, SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT);
}

// Draw health bars for both players
void drawHealthBars()
{
  const int BAR_HEIGHT = 4;
  const int BAR_MAX_WIDTH = 60;
  const int BAR_Y = 2;
  const int P1_BAR_X = 2;
  const int P2_BAR_X = 66;

  // Define colors
  uint16_t colorGreen = RGB565(0, 31, 0);   // Full green
  uint16_t colorYellow = RGB565(31, 63, 0); // Yellow
  uint16_t colorRed = RGB565(31, 0, 0);     // Red
  uint16_t colorDark = RGB565(4, 4, 4);     // Dark background

  // Player 1 health bar
  int p1HealthWidth = (player1.health * BAR_MAX_WIDTH) / 100;
  if (p1HealthWidth < 0) p1HealthWidth = 0;
  if (p1HealthWidth > BAR_MAX_WIDTH) p1HealthWidth = BAR_MAX_WIDTH;

  // Choose color based on health percentage
  uint16_t p1Color;
  if (player1.health > 60) {
    p1Color = colorGreen;
  } else if (player1.health > 30) {
    p1Color = colorYellow;
  } else {
    p1Color = colorRed;
  }

  // Draw P1 background (empty health)
  dma_display->fillRect(P1_BAR_X, BAR_Y, BAR_MAX_WIDTH, BAR_HEIGHT, colorDark);
  // Draw P1 foreground (remaining health)
  if (p1HealthWidth > 0) {
    dma_display->fillRect(P1_BAR_X, BAR_Y, p1HealthWidth, BAR_HEIGHT, p1Color);
  }

  // Player 2 health bar
  int p2HealthWidth = (player2.health * BAR_MAX_WIDTH) / 100;
  if (p2HealthWidth < 0) p2HealthWidth = 0;
  if (p2HealthWidth > BAR_MAX_WIDTH) p2HealthWidth = BAR_MAX_WIDTH;

  // Choose color based on health percentage
  uint16_t p2Color;
  if (player2.health > 60) {
    p2Color = colorGreen;
  } else if (player2.health > 30) {
    p2Color = colorYellow;
  } else {
    p2Color = colorRed;
  }

  // Draw P2 background (empty health)
  dma_display->fillRect(P2_BAR_X, BAR_Y, BAR_MAX_WIDTH, BAR_HEIGHT, colorDark);
  // Draw P2 foreground (remaining health)
  if (p2HealthWidth > 0) {
    dma_display->fillRect(P2_BAR_X, BAR_Y, p2HealthWidth, BAR_HEIGHT, p2Color);
  }
}

// Draw gamepad icon
// x, y: top-left position
// color: icon color (RED for P1, BLUE for P2)
void drawGamepadIcon(int x, int y, uint16_t color) {
  // Gamepad body (simplified controller shape)
  // Top shoulder buttons
  dma_display->fillRect(x + 1, y, 8, 1, color);

  // Main body
  dma_display->fillRect(x, y + 1, 10, 5, color);

  // D-pad (left side) - slightly darker
  dma_display->fillCircle(x + 2, y + 3, 1, 0x0000);

  // Buttons (right side) - slightly darker
  dma_display->fillCircle(x + 7, y + 3, 1, 0x0000);

  // Grips at bottom
  dma_display->fillRect(x, y + 6, 3, 1, color);
  dma_display->fillRect(x + 7, y + 6, 3, 1, color);
}

// Draw start menu
void drawMenu()
{
  dma_display->setTextSize(1);
  dma_display->setTextWrap(false);

  // Draw "HEROMAN" title centered near top - "HERO" in red, "MAN" in blue
  // "HEROMAN" is 7 characters * 6 pixels = 42 pixels wide
  // Center at (128-42)/2 = 43
  int titleY = 8;

  // Draw "HERO" with shadow
  dma_display->setCursor(44, titleY + 1);
  dma_display->setTextColor(RGB565(255, 255, 255)); // Black shadow
  dma_display->print("HERO");

  dma_display->setCursor(43, titleY);
  dma_display->setTextColor(RGB565(31, 0, 0)); // Red
  dma_display->print("HERO");

  // Draw "MAN" with shadow (4 chars * 6 pixels = 24 pixels after HERO)
  dma_display->setCursor(68, titleY + 1);
  dma_display->setTextColor(RGB565(255, 255, 255)); // Black shadow
  dma_display->print("MAN");

  dma_display->setCursor(67, titleY);
  dma_display->setTextColor(RGB565(0, 0, 31)); // Blue
  dma_display->print("MAN");

  // NOTE: Static player sprites removed - attract mode shows actual animated AI players fighting
  // The real players are drawn in drawFrame() before drawMenu() is called

  // Blink "PRESS A TO START" text
  menuBlinkTimer++;
  if ((menuBlinkTimer / 15) % 2 == 0) { // Blink every 15 frames
    dma_display->setCursor(20, 48);
    dma_display->setTextColor(RGB565(31, 63, 0)); // Yellow
    dma_display->print("PRESS A TO START");
  }

  // Draw gamepad icons ONLY when controllers are connected
  // Position icons near screen edges
  // Player 1 icon: GREEN - far left
  if (player1Gamepad != nullptr && player1Gamepad->isConnected()) {
    drawGamepadIcon(5, titleY, COLOR_GREEN);
  }

  // Player 2 icon: GREEN - far right
  if (player2Gamepad != nullptr && player2Gamepad->isConnected()) {
    drawGamepadIcon(113, titleY, COLOR_GREEN);
  }

}

// Draw "PLAYER X WINS" text at top of screen
void drawVictoryText()
{
  if (winner == nullptr) return;

  // Simple text drawing using Adafruit GFX (built into the library)
  dma_display->setTextSize(1);
  dma_display->setTextWrap(false);

  // Draw text shadow for visibility
  dma_display->setCursor(20, 10);
  dma_display->setTextColor(RGB565(0, 0, 0)); // Black shadow
  dma_display->print("PLAYER ");
  dma_display->print(winner->playerNumber);
  dma_display->print(" WINS!");

  // Draw main text
  dma_display->setCursor(19, 9);
  uint16_t textColor = (winner->playerNumber == 1) ? RGB565(31, 0, 0) : RGB565(0, 0, 31); // Red or Blue
  dma_display->setTextColor(textColor);
  dma_display->print("PLAYER ");
  dma_display->print(winner->playerNumber);
  dma_display->print(" WINS!");
}

// Draw countdown sequence "Ready! Set! Fight!"
void drawCountdown()
{
  dma_display->setTextSize(2);  // Larger text for countdown
  dma_display->setTextWrap(false);

  const char* message = "";
  uint16_t textColor = RGB565(31, 31, 31);  // White
  int xOffset = 0;

  // Determine which message to show based on stateTimer
  // Frames 0-40: "READY!" (1.6 seconds)
  // Frames 41-80: "SET!" (1.6 seconds)
  // Frames 81-105: "FIGHT!" (1.0 second)

  if (stateTimer < 41) {
    message = "READY!";
    textColor = RGB565(31, 31, 0);  // Yellow
    xOffset = 20;  // Center "READY!" (6 chars * 12 pixels = 72, center at (128-72)/2 = 28)
  } else if (stateTimer < 81) {
    message = "SET!";
    textColor = RGB565(31, 15, 0);  // Orange
    xOffset = 38;  // Center "SET!" (4 chars * 12 pixels = 48, center at (128-48)/2 = 40)
  } else {
    message = "FIGHT!";
    textColor = RGB565(31, 0, 0);  // Red
    xOffset = 20;  // Center "FIGHT!" (6 chars * 12 pixels = 72, center at (128-72)/2 = 28)
  }

  // Draw shadow for depth
  int yPos = 26;  // Center vertically
  dma_display->setCursor(xOffset + 1, yPos + 1);
  dma_display->setTextColor(RGB565(0, 0, 0));  // Black shadow
  dma_display->print(message);

  // Draw main text
  dma_display->setCursor(xOffset, yPos);
  dma_display->setTextColor(textColor);
  dma_display->print(message);
}

// Draw both players and the floor
void drawFrame()
{
  // Single buffer mode - no flip needed
  dma_display->flipDMABuffer(); // Only needed with double buffering


  static int frameCount = 0;
  frameCount++;

  // Check positions BEFORE delay
  //if (player2.xPos < -100 || player2.xPos > 200) {
  //  Serial.printf("CORRUPTION BEFORE delay(4)! Frame %d, P2 xPos=%d\n", frameCount, player2.xPos);
  //}

  delay(4);

  // Check positions AFTER delay, BEFORE clearScreen
  //if (player2.xPos < -100 || player2.xPos > 200) {
  //  Serial.printf("CORRUPTION AFTER delay, BEFORE clearScreen! Frame %d, P2 xPos=%d\n", frameCount, player2.xPos);
  //}

  dma_display->clearScreen();

  // Check positions AFTER clearScreen, BEFORE drawing
  //if (player2.xPos < -100 || player2.xPos > 200) {
  //  Serial.printf("CORRUPTION AFTER clearScreen! Frame %d, P2 xPos=%d\n", frameCount, player2.xPos);
  //}

  // Debug: Validate positions before drawing
  //if (frameCount % 60 == 0) {  // Log every ~2.4 seconds
    //Serial.printf("Frame %d - P1 xPos=%d, P2 xPos=%d\n", frameCount, player1.xPos, player2.xPos);
  //}

  if (gameState == GAME_MENU) {
    // Draw attract mode: CPU vs CPU demo fighting
    // Draw health bars at top
    drawHealthBars();

    // Draw both players fighting
    drawPlayer(&player1);
    drawPlayer(&player2);

    // Draw floor
    u_int16_t grey = 12645;
    dma_display->fillRect(0, screen_Height-STONEWALL_HEIGHT, screen_Width, STONEWALL_HEIGHT, grey);

    // Draw menu screen on top of the fighting
    drawMenu();
  } else if (gameState == GAME_COUNTDOWN) {
    // Draw countdown sequence
    // Draw health bars at top
    drawHealthBars();

    // Draw both players in idle stance
    drawPlayer(&player1);
    drawPlayer(&player2);

    // Draw floor
    u_int16_t grey = 12645;
    dma_display->fillRect(0, screen_Height-STONEWALL_HEIGHT, screen_Width, STONEWALL_HEIGHT, grey);

    // Draw countdown text overlay
    drawCountdown();
  } else {
    // Draw gameplay
    // Draw health bars at top
    drawHealthBars();

    // Draw both players - during victory sequence, draw winner last (on top)
    if (gameState >= GAME_VICTORY_WALK && gameState <= GAME_RESET_COUNTDOWN) {
      // Victory sequence - draw loser first, then winner on top
      drawPlayer(loser);
      drawPlayer(winner);
    } else {
      // Normal gameplay - player1 first, player2 on top
      drawPlayer(&player1);
      drawPlayer(&player2);
    }

    // Draw floor
    u_int16_t grey = 12645;
    dma_display->fillRect(0, screen_Height-STONEWALL_HEIGHT, screen_Width, STONEWALL_HEIGHT, grey);

    // Draw game state overlays
    if (gameState >= GAME_FREEZE_FRAME) {
      drawVictoryText();
    }
  }
}

// Initialize a single player
void initPlayer(Player* p, int playerNumber, int startX, const unsigned short** sprites)
{
  p->playerNumber = playerNumber;
  p->xPos = startX;  // Direct assignment OK during init
  Serial.printf("initPlayer: Player %d initialized at xPos=%d\n", p->playerNumber, p->xPos);
  p->yPos = screen_Height - STONEWALL_HEIGHT - SPRITE_FRAME_HEIGHT;
  p->direction = MovingRight;
  p->speed = 0;
  p->animation = ANIMATION_STOPPED;
  p->animationFrameIndex = 0;
  p->animationDelayCounter = 0;  // Initialize animation delay counter
  p->animationFrameset = player_animation_index_stopped;
  p->ctrlState = 0;
  p->spriteFrames = sprites;
  p->imgIndex = 0;

  // Initialize combat state
  p->health = 100;
  p->isHit = false;
  p->hitCooldown = 0;
  p->canPunch = true;
  p->punchCooldown = 0;
  p->isDead = false;

  // Initialize pushback collision
  p->pushbackCooldown = 0;
}

// Check if attacker is facing victim
bool isFacing(Player* attacker, Player* victim)
{
  // If attacker is to the left of victim, must be facing right
  if (attacker->xPos < victim->xPos) {
    return attacker->direction == MovingRight;
  }
  // If attacker is to the right of victim, must be facing left
  else {
    return attacker->direction == MovingLeft;
  }
}

// Check if player is in a squat/crouching animation
bool isSquatting(Player* p)
{
  return (p->animation == ANIMATION_SQUATTING ||
          p->animation == ANIMATION_SQUATPUNCHING_HIGH ||
          p->animation == ANIMATION_SQUATPUNCHING_LOW ||
          p->animation == ANIMATION_SQUATBLOCKING);
}

// Check for combat interactions between players
void checkCombat()
{
  // Check if player1 is punching and colliding with player2
  // Player 1 must be facing Player 2 to register a hit
  // IMPORTANT: Victim must not already be in hit stagger (invincibility frames)
  bool p1Punching = (player1.animation == ANIMATION_PUNCHING_HIGH ||
                     player1.animation == ANIMATION_PUNCHING_LOW ||
                     player1.animation == ANIMATION_SQUATPUNCHING_HIGH ||
                     player1.animation == ANIMATION_SQUATPUNCHING_LOW);

  if (p1Punching) {
    bool facing = isFacing(&player1, &player2);
    bool colliding = checkCollision(&player1, &player2);
    bool invuln = player2.isHit;

    Serial.printf("P1 PUNCH: facing=%d collision=%d p2.isHit=%d\n", facing, colliding, invuln);

    if (facing && colliding && !invuln) {
      // Check for squat avoidance: high punches miss squatting opponents
      bool isHighPunch = (player1.animation == ANIMATION_PUNCHING_HIGH);
      bool victimSquatting = isSquatting(&player2);

      // High punch misses squatting victim (punch goes over their head)
      if (isHighPunch && victimSquatting) {
        Serial.println("  -> MISS (squat dodge)");
      } else {
        // Hit connects!
        Serial.println("  -> HIT!");
        bool isLowPunch = (player1.animation == ANIMATION_PUNCHING_LOW ||
                           player1.animation == ANIMATION_SQUATPUNCHING_LOW ||
                           player1.animation == ANIMATION_SQUATPUNCHING_HIGH);
        applyHit(&player2, &player1, isLowPunch);
      }
    }
  }

  // Check if player2 is punching and colliding with player1
  // Player 2 must be facing Player 1 to register a hit
  // IMPORTANT: Victim must not already be in hit stagger (invincibility frames)
  bool p2Punching = (player2.animation == ANIMATION_PUNCHING_HIGH ||
                     player2.animation == ANIMATION_PUNCHING_LOW ||
                     player2.animation == ANIMATION_SQUATPUNCHING_HIGH ||
                     player2.animation == ANIMATION_SQUATPUNCHING_LOW);

  if (p2Punching) {
    bool facing = isFacing(&player2, &player1);
    bool colliding = checkCollision(&player2, &player1);
    bool invuln = player1.isHit;

    Serial.printf("P2 PUNCH: facing=%d collision=%d p1.isHit=%d\n", facing, colliding, invuln);

    if (facing && colliding && !invuln) {
      // Check for squat avoidance: high punches miss squatting opponents
      bool isHighPunch = (player2.animation == ANIMATION_PUNCHING_HIGH);
      bool victimSquatting = isSquatting(&player1);

      // High punch misses squatting victim (punch goes over their head)
      if (isHighPunch && victimSquatting) {
        Serial.println("  -> MISS (squat dodge)");
      } else {
        // Hit connects!
        Serial.println("  -> HIT!");
        bool isLowPunch = (player2.animation == ANIMATION_PUNCHING_LOW ||
                           player2.animation == ANIMATION_SQUATPUNCHING_LOW ||
                           player2.animation == ANIMATION_SQUATPUNCHING_HIGH);
        applyHit(&player1, &player2, isLowPunch);
      }
    }
  }
}

// Simple collision to reduce excessive overlap
// Players can pass through each other occasionally (like classic fighting games)
void checkPlayerPushback()
{
  #define SPRITE_TRANSPARENT_BORDER 8  // Pixels of transparency on each side of sprite

  // Calculate actual character positions (inset from sprite edges due to transparency)
  int p1LeftEdge = player1.xPos + SPRITE_TRANSPARENT_BORDER;
  int p1RightEdge = player1.xPos + SPRITE_FRAME_WIDTH - SPRITE_TRANSPARENT_BORDER;
  int p2LeftEdge = player2.xPos + SPRITE_TRANSPARENT_BORDER;
  int p2RightEdge = player2.xPos + SPRITE_FRAME_WIDTH - SPRITE_TRANSPARENT_BORDER;

  // Calculate overlap
  int overlap = 0;
  if (p1LeftEdge < p2LeftEdge) {
    // Player 1 is on the left
    overlap = p1RightEdge - p2LeftEdge;
  } else {
    // Player 2 is on the left
    overlap = p2RightEdge - p1LeftEdge;
  }

  // Push back on any overlap to prevent passing through
  if (overlap > 3) {
    // Push both players back from each other
    int pushAmount = 2;  // Enough to prevent circling but gentle enough for fighting

    if (player1.xPos < player2.xPos) {
      // P1 on left, P2 on right - push them apart
      setPlayerXPos(&player1, player1.xPos - pushAmount, "pushback");
      setPlayerXPos(&player2, player2.xPos + pushAmount, "pushback");
    } else {
      // P2 on left, P1 on right - push them apart
      setPlayerXPos(&player2, player2.xPos - pushAmount, "pushback");
      setPlayerXPos(&player1, player1.xPos + pushAmount, "pushback");
    }
  }
}

// Update game state machine
void updateGameState()
{
  switch (gameState) {
    case GAME_MENU:
      // CPU vs CPU demo mode runs until a real player presses start
      {
        // Check for REAL gamepad button presses (not AI)
        bool p1RealPress = false;
        bool p2RealPress = false;

        if (player1Gamepad != nullptr && player1Gamepad->isConnected()) {
          if (player1Gamepad->buttons() & BUTTON_A) {
            p1RealPress = true;
          }
        }

        if (player2Gamepad != nullptr && player2Gamepad->isConnected()) {
          if (player2Gamepad->buttons() & BUTTON_A) {
            p2RealPress = true;
          }
        }

        if (p1RealPress || p2RealPress) {
          // Real player(s) pressed start - begin countdown sequence
          Serial.println("Starting countdown...");
          gameState = GAME_COUNTDOWN;
          stateTimer = 0;  // Reset timer for countdown
          menuBlinkTimer = 0;

          // Disable AI for players with connected gamepads
          if (player1Gamepad != nullptr && player1Gamepad->isConnected()) {
            disableAI(&aiPlayer1);
            Serial.println("Player 1 gamepad detected - AI disabled for P1");
          } else {
            enableAI(&aiPlayer1);
            Serial.println("No Player 1 gamepad - AI remains enabled for P1");
          }

          if (player2Gamepad != nullptr && player2Gamepad->isConnected()) {
            disableAI(&aiPlayer2);
            Serial.println("Player 2 gamepad detected - AI disabled for P2");
          } else {
            enableAI(&aiPlayer2);
            Serial.println("No Player 2 gamepad - AI remains enabled for P2");
          }

          // Initialize both players
          initPlayer(&player1, 1, 10, player_right_frames);
          initPlayer(&player2, 2, screen_Width - 60, player_right_blue_frames);
        }
      }
      break;

    case GAME_COUNTDOWN:
      // Display "Ready! Set! Fight!" countdown sequence
      {
        stateTimer++;

        // Countdown timing (at ~25 FPS):
        // Frames 0-40: "READY!" (1.6 seconds)
        // Frames 41-80: "SET!" (1.6 seconds)
        // Frames 81-105: "FIGHT!" (1.0 second)
        // Frame 106+: Transition to GAME_PLAYING

        if (stateTimer >= 106) {
          // Countdown complete - start game!
          Serial.println("Countdown complete - FIGHT!");
          gameState = GAME_PLAYING;
          stateTimer = 0;
        }
      }
      break;

    case GAME_PLAYING:
      // Normal gameplay - check for home button press to return to menu
      {
        uint32_t p1State = controller_read_state(1);
        uint32_t p2State = controller_read_state(2);

        // If either player presses home/start button, return to menu
        if (BUTTON_PRESSED(START_BTN, p1State) || BUTTON_PRESSED(START_BTN, p2State)) {
          Serial.println("Home button pressed, returning to menu...");
          gameState = GAME_MENU;
          winner = nullptr;
          loser = nullptr;
          menuBlinkTimer = 0;

          // Re-enable AI for attract mode (CPU vs CPU demo)
          enableAI(&aiPlayer1);
          enableAI(&aiPlayer2);
          Serial.println("Attract mode resumed - CPU vs CPU demo");
        }
      }
      break;

    case GAME_FREEZE_FRAME:
      // Freeze frame on killing blow
      stateTimer--;
      if (stateTimer <= 0) {
        // Transition to victory pause
        gameState = GAME_VICTORY_PAUSE;
        stateTimer = VICTORY_PAUSE_DELAY;
        Serial.println("Freeze frame ended, starting victory pause");
      }
      break;

    case GAME_VICTORY_PAUSE:
      // Brief pause before winner walks
      stateTimer--;
      if (stateTimer <= 0) {
        // Transition to victory walk
        gameState = GAME_VICTORY_WALK;
        winner->speed = VICTORY_WALK_SPEED;
        setAnimation(winner, ANIMATION_RUNNING, player_animation_index_running);

        // Determine which side winner starts on and lock it in
        winnerStartedOnLeft = (winner->xPos < loser->xPos);
        Serial.printf("Victory pause ended, winner starting walk (startedOnLeft=%d)\n", winnerStartedOnLeft);
      }
      break;

    case GAME_VICTORY_WALK:
      {
        // Winner walks to stand over the loser's head (far edge of sprite)
        // Use the starting position to determine direction, not current position
        static int debugCounter = 0;
        debugCounter++;

        int targetX;
        if (winnerStartedOnLeft) {
          // Winner started on left, walk right to stand over loser's head (right side)
          // Loser's head is at the far edge, so walk to loser + half sprite width
          targetX = loser->xPos + (SPRITE_FRAME_WIDTH / 4);
          winner->direction = MovingRight;
          int newPos = winner->xPos + VICTORY_WALK_SPEED;

          // Debug every 10 frames
          if (debugCounter % 10 == 0) {
            Serial.printf("WALK: winner=%d, loser=%d, target=%d, newPos=%d\n",
                         winner->xPos, loser->xPos, targetX, newPos);
          }

          if (newPos >= targetX) {
            newPos = targetX;
            // Reached target - start taunt
            gameState = GAME_VICTORY_TAUNT;
            victorySquatCount = 0;
            stateTimer = 0;
            setAnimation(winner, ANIMATION_SQUATTING, player_animation_index_squatting);
            Serial.printf("Winner reached loser at position %d, starting taunt\n", newPos);
            debugCounter = 0;
            // TODO: Later feature - make winner face camera (turn to face right if P1, left if P2)
          }
          setPlayerXPos(winner, newPos, "victoryWalk");
        } else {
          // Winner started on right, walk left to stand over loser's head (left side)
          // Loser's head is at the far edge, so walk to loser - half sprite width
          targetX = loser->xPos - (SPRITE_FRAME_WIDTH / 4);
          winner->direction = MovingLeft;
          int newPos = winner->xPos - VICTORY_WALK_SPEED;

          // Debug every 10 frames
          if (debugCounter % 10 == 0) {
            Serial.printf("WALK: winner=%d, loser=%d, target=%d, newPos=%d\n",
                         winner->xPos, loser->xPos, targetX, newPos);
          }

          if (newPos <= targetX) {
            newPos = targetX;
            // Reached target - start taunt
            gameState = GAME_VICTORY_TAUNT;
            victorySquatCount = 0;
            stateTimer = 0;
            setAnimation(winner, ANIMATION_SQUATTING, player_animation_index_squatting);
            Serial.printf("Winner reached loser at position %d, starting taunt\n", newPos);
            debugCounter = 0;
            // TODO: Later feature - make winner face camera (turn to face right if P1, left if P2)
          }
          setPlayerXPos(winner, newPos, "victoryWalk");
        }

        // Advance walking animation
        winner->animationFrameIndex = calcNextAnimationIndex(winner);
      }
      break;

    case GAME_VICTORY_TAUNT:
      // Do 4 squats (toggle between squat and stand)
      stateTimer++;
      if (stateTimer >= SQUAT_HOLD_FRAMES) {
        stateTimer = 0;

        // Toggle animation between squat and stand
        if (winner->animation == ANIMATION_SQUATTING) {
          setAnimation(winner, ANIMATION_STOPPED, player_animation_index_stopped);
        } else {
          setAnimation(winner, ANIMATION_SQUATTING, player_animation_index_squatting);
          victorySquatCount++;
          Serial.printf("Squat %d/4 complete\n", victorySquatCount);

          if (victorySquatCount >= 4) {
            // Taunt complete - freeze for a moment before reset
            gameState = GAME_RESET_COUNTDOWN;
            stateTimer = RESET_FREEZE_DELAY;
            Serial.println("Taunt complete, freezing before reset");
          }
        }
      }
      break;

    case GAME_RESET_COUNTDOWN:
      // Freeze for a moment, then return to attract mode
      stateTimer--;
      if (stateTimer <= 0) {
        // Return to menu for attract mode (CPU vs CPU demo)
        Serial.println("Returning to attract mode...");
        gameState = GAME_MENU;
        winner = nullptr;
        loser = nullptr;
        menuBlinkTimer = 0;

        // Re-enable AI for attract mode
        enableAI(&aiPlayer1);
        enableAI(&aiPlayer2);

        // Reset both players
        initPlayer(&player1, 1, 10, player_right_frames);
        initPlayer(&player2, 2, screen_Width - 60, player_right_blue_frames);
      }
      break;
  }
}

void init_hero()
{
  Serial.println("Initializing hero game...");
  Serial.printf("Screen dimensions: width=%d, height=%d\n", screen_Width, screen_Height);
  Serial.printf("Sprite dimensions: width=%d, height=%d\n", SPRITE_FRAME_WIDTH, SPRITE_FRAME_HEIGHT);
  Serial.printf("Max player xPos: %d\n", screen_Width - SPRITE_FRAME_WIDTH);
  Serial.printf("Wrap boundaries: left=%d, right=%d\n", 0 - SPRITE_FRAME_WIDTH, screen_Width + SPRITE_FRAME_WIDTH);

  // Log memory addresses to verify guard buffer placement
  Serial.printf("Player 1 address: 0x%08X\n", (uint32_t)&player1);
  Serial.printf("Guard buffer address: 0x%08X\n", (uint32_t)&memoryGuard[0]);
  Serial.printf("Player 2 address: 0x%08X\n", (uint32_t)&player2);
  Serial.printf("Memory separation: %d bytes\n", (uint32_t)&player2 - (uint32_t)&player1);

  // Initialize controller
  initializeController();
  Serial.println("Controller initialized");

  // Initialize players for attract mode (CPU vs CPU demo)
  initPlayer(&player1, 1, 10, player_right_frames);
  initPlayer(&player2, 2, screen_Width - 60, player_right_blue_frames);
  Serial.println("Players initialized for attract mode");
  Serial.println("Starting at menu screen - CPU vs CPU demo running - press A to begin");
}