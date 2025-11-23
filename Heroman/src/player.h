#pragma once

#include <stdint.h>
#include "sprites.h"

enum Direction {
    Stopped = 0,
    MovingRight = 1,
    MovingLeft = 2
};

struct Player {
    // Position
    int xPos;
    int yPos;

    // Animation state
    int animation;
    int animationFrameIndex;
    int* animationFrameset;
    int animationDelayCounter;  // Counter to slow down animation frame rate
    Direction direction;

    // Movement
    int speed;
    int imgIndex;  // Current image index

    // Controller state
    uint32_t ctrlState;

    // Sprite data
    const unsigned short** spriteFrames;

    // Flip buffer (each player needs their own for sprite flipping)
    uint16_t flippedSpriteBuffer[SPRITE_FRAME_WIDTH * SPRITE_FRAME_HEIGHT];

    // Mask buffer (each player needs their own to prevent DMA corruption)
    // Size must be calculated row-by-row: bytesPerRow * height
    uint8_t maskBuffer[((SPRITE_FRAME_WIDTH + 7) / 8) * SPRITE_FRAME_HEIGHT];

    // Player identification
    int playerNumber; // 1 or 2

    // Combat state
    int health;  // Player health (0-100)
    bool isHit;  // Currently in hit stagger
    int hitCooldown;  // Frames remaining in hit stagger
    bool canPunch;  // Can throw a punch (prevents spam)
    int punchCooldown;  // Frames until can punch again
    bool isDead;  // Player is dead

    // Pushback collision
    int pushbackCooldown;  // Frames until can be pushed back again
};
