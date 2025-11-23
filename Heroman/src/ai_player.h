#pragma once

#include "player.h"
#include "controller.h"
#include <stdlib.h>

// AI Personality Types
enum AIPersonality {
    AI_AGGRESSIVE,   // Rushdown style, lots of attacks
    AI_DEFENSIVE,    // Blocks often, counterattacks
    AI_BALANCED,     // Mix of offense and defense
    AI_RANDOM        // Random actions (beginner AI)
};

// AI Controller State
struct AIController {
    bool enabled;              // Is AI active for this player?
    AIPersonality personality; // AI behavior type
    Player* self;              // Pointer to AI's player
    Player* opponent;          // Pointer to opponent player
    int decisionTimer;         // Frames until next decision
    uint32_t currentAction;    // Current button state being held
    int actionDuration;        // How long to hold current action
};

// Helper functions to enable/disable AI
// IMPORTANT: These must be defined BEFORE heroman.h is included
// because heroman.h uses these functions in its game state logic
inline void enableAI(AIController* ai) {
    if (ai) ai->enabled = true;
}

inline void disableAI(AIController* ai) {
    if (ai) ai->enabled = false;
}

inline bool isAIEnabled(AIController* ai) {
    return (ai && ai->enabled);
}

// Include heroman.h AFTER helper functions so they're available when heroman.h needs them
#include "heroman.h"

// Forward declarations
uint32_t ai_aggressive_behavior(AIController* ai, int distance, int* duration);
uint32_t ai_defensive_behavior(AIController* ai, int distance, int* duration);
uint32_t ai_balanced_behavior(AIController* ai, int distance, int* duration);
uint32_t ai_random_behavior(AIController* ai, int* duration);

// Helper function: Create button state from individual button bools
uint32_t createButtonState(bool up, bool down, bool left, bool right,
                          bool punch, bool lowPunch, bool block) {
    uint32_t state = 0xFFFFFFFF;  // Start with all buttons released (inverted logic)

    if (up) state &= ~(1 << UP_BTN);
    if (down) state &= ~(1 << DOWN_BTN);
    if (left) state &= ~(1 << LEFT_BTN);
    if (right) state &= ~(1 << RIGHT_BTN);
    if (punch) state &= ~(1 << PUNCH_BTN);
    if (lowPunch) state &= ~(1 << JUMP_BTN);
    if (block) state &= ~(1 << TRI_BTN);

    return state;
}

// AI Range Constants
#define PUNCH_RANGE 35          // Distance where punches can hit
#define CLOSE_RANGE 45          // Very close combat range
#define MEDIUM_RANGE 70         // Medium distance
#define SAFE_DEFENSIVE_RANGE 50 // Defensive AI preferred distance

// Helper function: Get distance to opponent
int getDistanceToOpponent(AIController* ai) {
    if (!ai->self || !ai->opponent) return 1000;  // Large distance if invalid
    return abs(ai->self->xPos - ai->opponent->xPos);
}

// Helper function: Check if opponent is attacking
bool isOpponentAttacking(AIController* ai) {
    if (!ai->opponent) return false;

    int anim = ai->opponent->animation;
    return (anim == ANIMATION_PUNCHING_HIGH ||
            anim == ANIMATION_PUNCHING_LOW ||
            anim == ANIMATION_SQUATPUNCHING_HIGH ||
            anim == ANIMATION_SQUATPUNCHING_LOW);
}

// Helper function: Check if we're in range to punch
bool isInPunchRange(AIController* ai, int distance) {
    return distance <= PUNCH_RANGE && ai->self->canPunch;
}

// Helper function: Check if opponent is vulnerable (hit stagger or mid-animation)
bool isOpponentVulnerable(AIController* ai) {
    if (!ai->opponent) return false;
    return ai->opponent->isHit || isOpponentAttacking(ai);
}

// Helper function: Should we block right now?
bool shouldBlock(AIController* ai, int distance) {
    // Block if opponent is attacking and close enough to hit us
    return isOpponentAttacking(ai) && distance <= PUNCH_RANGE + 10;
}

// Initialize AI for a player
void initAI(AIController* ai, Player* self, Player* opponent, AIPersonality personality) {
    ai->enabled = false;
    ai->personality = personality;
    ai->self = self;
    ai->opponent = opponent;
    ai->decisionTimer = 0;
    ai->currentAction = 0xFFFFFFFF;  // No buttons pressed
    ai->actionDuration = 0;
}

// Aggressive AI: Rushdown style - constantly pressures, attacks in range
uint32_t ai_aggressive_behavior(AIController* ai, int distance, int* duration) {
    bool moveRight = (ai->opponent->xPos > ai->self->xPos);

    // PRIORITY 1: Opponent is vulnerable (hit or attacking) and we're in range - PUNISH!
    if (isOpponentVulnerable(ai) && isInPunchRange(ai, distance)) {
        *duration = 6;
        // Mix high and low punches for variety
        bool useLowPunch = (rand() % 100 < 40);
        if (useLowPunch) {
            return createButtonState(false, true, false, false,
                                    false, true, false);  // Squat low punch
        } else {
            return createButtonState(false, false, false, false,
                                    true, false, false);  // High punch
        }
    }

    // PRIORITY 2: In punch range - ATTACK! (even if opponent isn't vulnerable)
    if (isInPunchRange(ai, distance)) {
        *duration = 6;
        bool useLowPunch = (rand() % 100 < 30);  // 30% low, 70% high
        if (useLowPunch) {
            return createButtonState(false, true, false, false,
                                    false, true, false);
        } else {
            return createButtonState(false, false, false, false,
                                    true, false, false);
        }
    }

    // PRIORITY 3: Close but not quite in range - move forward to close gap
    if (distance < CLOSE_RANGE) {
        *duration = 4;  // Short duration for quick adjustments
        return createButtonState(false, false, !moveRight, moveRight,
                                false, false, false);
    }

    // PRIORITY 4: Medium range - advance steadily
    if (distance < MEDIUM_RANGE) {
        *duration = 8;
        return createButtonState(false, false, !moveRight, moveRight,
                                false, false, false);
    }

    // PRIORITY 5: Far range - rush in quickly
    *duration = 10;
    return createButtonState(false, false, !moveRight, moveRight,
                            false, false, false);
}

// Defensive AI: Patient, blocks well, waits for opportunities to counterattack
uint32_t ai_defensive_behavior(AIController* ai, int distance, int* duration) {
    bool moveRight = (ai->opponent->xPos > ai->self->xPos);

    // PRIORITY 1: Opponent is attacking and in range - BLOCK!
    if (shouldBlock(ai, distance)) {
        *duration = 12;  // Hold block longer for safety
        bool isSquatPunch = (ai->opponent->animation == ANIMATION_SQUATPUNCHING_LOW ||
                            ai->opponent->animation == ANIMATION_SQUATPUNCHING_HIGH);
        return createButtonState(false, isSquatPunch, false, false,
                                false, false, true);  // Block (squat if needed)
    }

    // PRIORITY 2: Opponent is vulnerable and we're in perfect range - COUNTERATTACK!
    if (ai->opponent->isHit && isInPunchRange(ai, distance)) {
        *duration = 6;
        return createButtonState(false, false, false, false,
                                true, false, false);  // Quick counter punch
    }

    // PRIORITY 3: Too close to opponent - back away to safe distance
    bool tooClose = (distance < SAFE_DEFENSIVE_RANGE - 10);
    if (tooClose) {
        *duration = 6;
        return createButtonState(false, false, moveRight, !moveRight,
                                false, false, false);  // Retreat
    }

    // PRIORITY 4: Too far from opponent - advance to safe distance
    bool tooFar = (distance > SAFE_DEFENSIVE_RANGE + 15);
    if (tooFar) {
        *duration = 6;
        return createButtonState(false, false, !moveRight, moveRight,
                                false, false, false);  // Advance cautiously
    }

    // PRIORITY 5: At safe distance - occasional poke or slight adjustments
    // Defensive AI should still be active, not just stand still
    if (distance < PUNCH_RANGE + 5 && ai->self->canPunch && rand() % 100 < 20) {
        *duration = 5;
        return createButtonState(false, false, false, false,
                                true, false, false);  // Occasional poke
    }

    // At safe distance but not in poke range - make small movements for positioning
    if (rand() % 100 < 30) {
        *duration = 5;
        // 50/50 move closer or back up slightly
        bool moveForward = (rand() % 2 == 0);
        if (moveForward) {
            return createButtonState(false, false, !moveRight, moveRight,
                                    false, false, false);
        } else {
            return createButtonState(false, false, moveRight, !moveRight,
                                    false, false, false);
        }
    }

    // Default: Stay alert at safe distance
    *duration = 4;  // Short duration to react quickly
    return 0xFFFFFFFF;  // Idle, ready to react
}

// Balanced AI: Adaptable fighter - blocks threats, punishes mistakes, controls space
uint32_t ai_balanced_behavior(AIController* ai, int distance, int* duration) {
    bool moveRight = (ai->opponent->xPos > ai->self->xPos);

    // Debug
    static int balancedDebugCounter = 0;
    bool shouldDebug = (balancedDebugCounter++ % 60 == 0);
    if (shouldDebug) {
        Serial.printf("  BALANCED: dist=%d, moveRight=%d\n", distance, moveRight);
    }

    // PRIORITY 1: Opponent attacking close - BLOCK!
    if (shouldBlock(ai, distance)) {
        if (shouldDebug) Serial.println("    -> BLOCK");

        *duration = 10;
        bool isSquatPunch = (ai->opponent->animation == ANIMATION_SQUATPUNCHING_LOW ||
                            ai->opponent->animation == ANIMATION_SQUATPUNCHING_HIGH);
        return createButtonState(false, isSquatPunch, false, false,
                                false, false, true);
    }

    // PRIORITY 2: Opponent vulnerable and in range - PUNISH!
    if (isOpponentVulnerable(ai) && isInPunchRange(ai, distance)) {
        if (shouldDebug) Serial.println("    -> PUNISH");
        *duration = 6;
        bool useLowPunch = (rand() % 100 < 35);
        if (useLowPunch) {
            return createButtonState(false, true, false, false,
                                    false, true, false);
        } else {
            return createButtonState(false, false, false, false,
                                    true, false, false);
        }
    }

    // PRIORITY 3: Very close range - smart decision based on situation
    if (distance < CLOSE_RANGE) {
        if (shouldDebug) Serial.println("    -> CLOSE RANGE");
        // If we can punch and opponent not blocking, 60% attack
        if (isInPunchRange(ai, distance) && rand() % 100 < 60) {
            *duration = 6;
            return createButtonState(false, false, false, false,
                                    true, false, false);
        } else {
            // Otherwise back up a bit for spacing
            *duration = 5;
            return createButtonState(false, false, moveRight, !moveRight,
                                    false, false, false);
        }
    }

    // PRIORITY 4: Medium range - advance with occasional attacks
    if (distance < MEDIUM_RANGE) {
        if (shouldDebug) Serial.printf("    -> MEDIUM RANGE (dist=%d < %d)\n", distance, MEDIUM_RANGE);
        // 35% chance to attack if in range
        if (isInPunchRange(ai, distance) && rand() % 100 < 35) {
            *duration = 6;
            return createButtonState(false, false, false, false,
                                    true, false, false);
        } else {
            if (shouldDebug) Serial.printf("    -> ADVANCING (moveRight=%d)\n", moveRight);
            // Move forward
            *duration = 7;
            return createButtonState(false, false, !moveRight, moveRight,
                                    false, false, false);
        }
    }

    // PRIORITY 5: Far range - close the distance
    if (shouldDebug) Serial.println("    -> FAR RANGE");
    *duration = 8;
    return createButtonState(false, false, !moveRight, moveRight,
                            false, false, false);
}

// Random AI: Beginner level, random actions
uint32_t ai_random_behavior(AIController* ai, int* duration) {
    int action = rand() % 10;
    *duration = 5 + (rand() % 10);  // 5-15 frames

    bool moveRight = (ai->opponent->xPos > ai->self->xPos);

    switch (action) {
        case 0:  // Move toward opponent
            return createButtonState(false, false, !moveRight, moveRight,
                                    false, false, false);
        case 1:  // Move away from opponent
            return createButtonState(false, false, moveRight, !moveRight,
                                    false, false, false);
        case 2:  // Punch
            if (ai->self->canPunch) {
                return createButtonState(false, false, false, false,
                                        true, false, false);
            }
            break;
        case 3:  // Low punch
            if (ai->self->canPunch) {
                return createButtonState(false, false, false, false,
                                        false, true, false);
            }
            break;
        case 4:  // Block
            return createButtonState(false, false, false, false,
                                    false, false, true);
        case 5:  // Squat
            return createButtonState(false, true, false, false,
                                    false, false, false);
        default:  // Idle
            break;
    }

    return 0xFFFFFFFF;  // Idle
}

// Main AI decision function - called every frame when AI is enabled
uint32_t ai_make_decision(AIController* ai) {
    // Validate AI controller
    if (!ai || !ai->self || !ai->opponent) {
        Serial.println("AI: Invalid controller/player pointers");
        return 0xFFFFFFFF;  // Return idle state if invalid
    }

    // Debug: Log every 60 frames (~2.4 seconds at 25fps)
    static int debugCounter = 0;
    if (debugCounter++ % 60 == 0) {
        Serial.printf("AI P%d: enabled=%d, dist=%d, action=0x%08X, dur=%d\n",
                     ai->self->playerNumber, ai->enabled,
                     getDistanceToOpponent(ai), ai->currentAction, ai->actionDuration);
    }

    // If currently holding an action, continue holding it until duration expires
    if (ai->actionDuration > 0) {
        ai->actionDuration--;
        return ai->currentAction;
    }

    // Action completed - make new decision immediately (no random delays!)
    // Get distance to opponent for decision making
    int distance = getDistanceToOpponent(ai);

    // Make decision based on personality
    uint32_t newAction = 0xFFFFFFFF;
    int duration = 1;

    switch (ai->personality) {
        case AI_AGGRESSIVE:
            newAction = ai_aggressive_behavior(ai, distance, &duration);
            break;
        case AI_DEFENSIVE:
            newAction = ai_defensive_behavior(ai, distance, &duration);
            break;
        case AI_BALANCED:
            newAction = ai_balanced_behavior(ai, distance, &duration);
            break;
        case AI_RANDOM:
            newAction = ai_random_behavior(ai, &duration);
            break;
        default:
            newAction = 0xFFFFFFFF;  // Idle
            duration = 5;
            break;
    }

    // Store new action and duration
    ai->currentAction = newAction;
    ai->actionDuration = duration;

    // Debug: Log the new decision
    if (debugCounter % 60 == 1) {  // Log right after the previous debug
        Serial.printf("  -> NEW decision: action=0x%08X, dur=%d, personality=%d\n",
                     newAction, duration, ai->personality);
    }

    return newAction;
}
