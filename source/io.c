#include "io.h"

#include "constants.h"
#include "logger.h"
#include "physics.h"
#include "rest.h"

#include <curses.h>
#include <string.h>


/**
 * Initializes the color schemes used to render the game.
 */
int initialize_color_schemes(void) {
    init_pair(TOP_BAR_COLOR, COLOR_CYAN, COLOR_MAGENTA);
    init_pair(PLATFORM_COLOR, COLOR_WHITE, COLOR_WHITE);
    init_pair(BOTTOM_BAR_COLOR, COLOR_BLACK, COLOR_YELLOW);
    return 0;
}

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 */
void initialize(void) {
    initialize_logger();
    // Initialize the screen.
    initscr();
    // Prevent terminal echo.
    noecho();
    // Prevent delay from getch().
    nodelay(stdscr, TRUE);
    // Do not display the cursor.
    curs_set(FALSE);
    // Initialize the coloring functionality.
    start_color();
    initialize_color_schemes();
}

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exitting.
 */
void finalize(void) {
    finalize_logger();
    endwin();
}

/**
 * Prints the provided string on the screen starting at (x, y).
 */
void print(const int x, const int y, const char *string) {
    // Validate that x and y are nonnegative.
    if (x < 0 || y < 0) {
        return;
    }
    mvprintw(y, x, string);
}

/**
 * Prints the provided string centered on the screen at the provided line.
 */
void print_centered(const int y, const char *string) {
    const int x = (COLS - strlen(string)) / 2;
    print(x, y, string);
}

/**
 * Prints the provided Platform, respecting the BoundingBox.
 */
void print_platform(const Platform * const platform, const BoundingBox * const box) {
    int i;
    for (i = 0; i < platform->width; i++) {
        const int x = platform->x + i;
        const int y = platform->y;
        if (x >= box->min_x && x <= box->max_x && y >= box->min_y && y <= box->max_y) {
            print(x, y, " ");
        }
    }
}

/**
 * Draws the top status bar on the screen for a given Player.
 *
 * Returns 0 if successful.
 */
int draw_top_bar(const Player * const player) {
    const int padding = 1; // How many spaces should surround the value (at least).
    const size_t columns_per_value = COLS / TOP_BAR_STRING_COUNT;

    char power_buffer[MAXIMUM_STRING_SIZE];
    sprintf(power_buffer, "Power: %d", 0); // Use a proper label here in the future.

    char lives_buffer[MAXIMUM_STRING_SIZE];
    sprintf(lives_buffer, "Lives: %d", player->lives); // Could use a repeated character.

    char score_buffer[MAXIMUM_STRING_SIZE];
    sprintf(score_buffer, "Score: %d", player->score);

    char *strings[TOP_BAR_STRING_COUNT] = {GAME_NAME, power_buffer, lives_buffer, score_buffer};

    size_t i = 0;

    // Check that there are enough columns.
    const size_t maximum_columns_per_string = columns_per_value - 2 * padding;
    for (i = 0; i < TOP_BAR_STRING_COUNT; i++) {
        if (strlen(strings[i]) > maximum_columns_per_string) {
            return 1;
        }
    }

    // Effectively write the strings.
    // Build a buffer with everything, this is best as it will give us the colored background for empty spaces easily.
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);
    final_buffer[COLS] = '\0';

    for (i = 0; i < TOP_BAR_STRING_COUNT; i++) {
        const size_t centering_padding = (maximum_columns_per_string - strlen(strings[i])) / 2;
        const size_t x = i * columns_per_value + padding + centering_padding;
        memcpy(final_buffer + x, strings[i], strlen(strings[i]));
    }

    attron(COLOR_PAIR(TOP_BAR_COLOR));
    attron(A_BOLD);
    print(0, 0, final_buffer);
    attroff(A_BOLD);
    attroff(COLOR_PAIR(TOP_BAR_COLOR));

    return 0;
}

/**
 * Draws the bottom status bar on the screen for a given Player.
 *
 * Returns 0 if successful.
 */
int draw_bottom_bar(void) {
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);
    attron(COLOR_PAIR(BOTTOM_BAR_COLOR));
    mvprintw(LINES - 1, 0, final_buffer);
    attroff(COLOR_PAIR(BOTTOM_BAR_COLOR));
    return 0;
}

int draw_borders(void) {
    int i;
    for (i = 1; i < COLS - 1; i++) {
        print(i, 1, "+");
    }
    for (i = 1; i < LINES - 1; i++) {
        print(0, i, "+");
    }
    for (i = 1; i < COLS - 1; i++) {
        print(i, LINES - 2, "+");
    }
    for (i = 1; i < LINES - 1; i++) {
        print(COLS - 1, i, "+");
    }
    return 0;
}

int draw_platforms(const Platform * platforms, const size_t platform_count, const BoundingBox * const box) {
    size_t i;
    attron(COLOR_PAIR(PLATFORM_COLOR));
    for (i = 0; i < platform_count; i++) {
        print_platform(&platforms[i], box);
    }
    attroff(COLOR_PAIR(PLATFORM_COLOR));
    return 0;
}

int draw_player(const Player * const player) {
    print(player->x, player->y, PLAYER_SYMBOL);
    return 0;
}

/**
 * Draws a full game to the screen.
 */
int draw(const Player * const player, const Platform *platforms, const size_t platform_count, const BoundingBox * const box) {
    clear();
  
    draw_top_bar(player);
    draw_bottom_bar();
    draw_borders();
    draw_platforms(platforms, platform_count, box);
    draw_player(player);
    
    refresh();
   
    return 0;
}

Command command_from_input(const int input) {
    if (input == '8') {
        return COMMAND_UP;
    } else if (input == '4') {
        return COMMAND_LEFT;
    } else if (input == '5') {
        return COMMAND_CENTER;
    } else if (input == '6') {
        return COMMAND_RIGHT;
    } else if (input == '2') {
        return COMMAND_DOWN;
    } else if (input == 'q' || input == 'Q') {
        return COMMAND_QUIT;
    } else if (input == ' ') {
        return COMMAND_JUMP;
    } else if (input == '\n') {
        return COMMAND_ENTER;
    } else {
        return NO_COMMAND;
    }
}

/**
 * Reads the next command that needs to be processed. This is the last command
 * on the input buffer.
 *
 * This function consumes the whole input buffer and returns either NO_COMMAND
 * (if no other Command could be produced by what was in the input buffer) or
 * the last Command different than NO_COMMAND that could be produced by what
 * was in the input buffer.
 */
Command read_next_command(void) {
    Command last_valid_command = NO_COMMAND;
    int input;
    for (input = getch(); input != ERR; input = getch()) {
        const Command current = command_from_input(input);
        if (current != NO_COMMAND) {
            last_valid_command = current;
        }
    }
    return last_valid_command;
}

/**
 * Waits for user input, indefinitely.
 */
Command wait_for_next_command(void) {
    Command command = NO_COMMAND;
    while (command == NO_COMMAND) {
        rest_for_second_fraction(MENU_FPS);
        command = read_next_command();
    }
    return command;
}

