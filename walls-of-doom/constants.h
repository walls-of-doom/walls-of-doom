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
 * The number of columns and lines of the window.
 */

#define COLUMNS 80

#define LINES 30

/**
 * How many spaces should be left from the margins when printing text.
 */
#define PADDING 2

/**
 * All stack character buffers default to this size.
 *
 * This is the maximum size which the application strings should have.
 *
 * Several functions hold arrays of this size. Therefore, increasing it will
 * likely dramatically increase the usage of the stack.
 */
#define MAXIMUM_STRING_SIZE 256

#define MAXIMUM_PLAYER_NAME_SIZE 64

#define FALLBACK_PLAYER_NAME "Player"

/**
 * The maximum number of platforms the game loads.
 */
#define MAXIMUM_PLATFORM_COUNT 256

/**
 * The number of frames per second the engine renders.
 */
#define FPS 30

/**
 * The base speed of the platforms. This number may be multiplied by up to 3.
 */
#define PLATFORM_BASE_SPEED 4

#define PLAYER_RUNNING_SPEED 9

#define PLAYER_FALLING_SPEED 12

#define PLAYER_JUMPING_SPEED PLAYER_FALLING_SPEED

#define PLAYER_JUMPING_HEIGHT PLAYER_JUMPING_SPEED

#define RECORD_ARRAY_SIZE 512

/**
 * The maximum number of records that will be displayed to the player.
 *
 * May be more than the maximum available.
 */
#define MAXIMUM_DISPLAYED_RECORDS 64

#define RECORD_TABLE_FILENAME "records.bin"

#define ABOUT_PAGE_BUFFER_SIZE 1024
#define ABOUT_PAGE_PATH "assets/about.txt"

/**
 * Information required for font loading.
 */
#define MONOSPACED_FONT_PATH "assets/fonts/roboto-mono/RobotoMono-Regular.ttf"
#define MONOSPACED_FONT_SIZE 20

#define ICON_PATH "assets/icons/icon-128.png"

#endif
