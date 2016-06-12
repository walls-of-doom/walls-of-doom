#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * This header file defines most of the constants used in the application.
 *
 * Most configuration tweaks require only small changes to this file.
 */
#define GAME_NAME "Walls of Doom"

#define PLAYER_SYMBOL "@"

/**
 * How many strings the top bar has.
 */
#define TOP_BAR_STRING_COUNT 4

/**
 * All stack character buffers default to this size.
 *
 * This is the maximum size which the application strings should have.
 */
#define MAXIMUM_STRING_SIZE 256

/**
 * The maximum number of platforms the game loads.
 */
#define MAXIMUM_PLATFORM_COUNT 32

/**
 * The number of frames per second the engine renders.
 */
#define FPS 30

/**
 * The base speed of the platforms. This number may be multiplied by up to 3.
 */
#define PLATFORM_BASE_SPEED 2

#define PLAYER_RUNNING_SPEED 4

#define PLAYER_FALLING_SPEED 8

#define PLAYER_JUMPING_SPEED PLAYER_FALLING_SPEED

#define PLAYER_JUMPING_HEIGHT 2 * PLAYER_JUMPING_SPEED

#endif
