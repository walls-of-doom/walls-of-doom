#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "rest.h"

#define GAME_NAME "Walls of Doom"
#define TOP_BAR_STRING_COUNT 4

#define PLAYER_SYMBOL "@"

enum COLOR_SCHEME {
    // Color pair 0 is assumed to be white on black, but is actually whatever
    // the terminal implements before color is initialized. It cannot be
    // modified by the application, therefore we must start at 1.
    TOP_BAR_COLOR = 1, // Set the first enum constant to one
    BOTTOM_BAR_COLOR // Becomes two, and so on
};

int initialize_color_schemes() {
    init_pair(TOP_BAR_COLOR, COLOR_MAGENTA, COLOR_CYAN);
    init_pair(BOTTOM_BAR_COLOR, COLOR_BLACK, COLOR_YELLOW);
    return 0;
}

typedef struct Player {
    char *name;
    // Could let the color of the player lay here, but why?
    unsigned int lives;
    unsigned int score;
} Player;

Player make_player(char *name) {
    Player player;
    player.name = name;
    player.lives = 3;
    player.score = 0;
    return player;
}

/**
 * Writes the top status bar on the screen for a given Player.
 *
 * Returns 0 if successful.
 */
int write_top_bar(const Player * const player) {
    const int padding = 1; // How many spaces should surround the value (at least).
    const int columns_per_value = COLS / TOP_BAR_STRING_COUNT;

    char power_buffer[64];
    sprintf(power_buffer, "Power: %d", 0); // Use a proper label here in the future.

    char lives_buffer[64];
    sprintf(lives_buffer, "Lives: %d", player->lives); // Could use a repeated character.

    char score_buffer[64];
    sprintf(score_buffer, "Score: %d", player->score);

    char *strings[TOP_BAR_STRING_COUNT] = {GAME_NAME, power_buffer, lives_buffer, score_buffer};

    // Check that there are enough columns.
    const int maximum_columns_per_string = columns_per_value - 2 * padding;
    for (size_t i = 0; i < TOP_BAR_STRING_COUNT; i++) {
        if (strlen(strings[i]) > maximum_columns_per_string) {
            return 1;
        }
    }

    // Effectively write the strings.
    // Build a buffer with everything, this is best as it will give us the colored background for empty spaces easily.
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);
    final_buffer[COLS] = '\0';

    for (size_t i = 0; i < TOP_BAR_STRING_COUNT; i++) {
        const int centering_padding = (maximum_columns_per_string - strlen(strings[i])) / 2;
        const int x = i * columns_per_value + padding + centering_padding;
        memcpy(final_buffer + x, strings[i], strlen(strings[i]));
    }

    attron(COLOR_PAIR(TOP_BAR_COLOR));
    mvprintw(0, 0, final_buffer);
    attroff(COLOR_PAIR(TOP_BAR_COLOR));

    return 0;
}

int write_player(const Player * const player) {
    mvprintw(LINES / 2, COLS / 2, PLAYER_SYMBOL);
}

int write_bottom_bar() {
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);

    attron(COLOR_PAIR(BOTTOM_BAR_COLOR));
    mvprintw(LINES - 1, 0, final_buffer);
    attroff(COLOR_PAIR(BOTTOM_BAR_COLOR));
}

int main() {
    // Initialize the screen.
    initscr();
    // Prevent terminal echo.
    noecho();
    // Do not display the cursor.
    curs_set(FALSE);
    // Initialize the coloring functionality.
    start_color();
    initialize_color_schemes();

    Player player = make_player("Dude");

    write_top_bar(&player);
    write_player(&player);
    write_bottom_bar();

    refresh();
    rest_for_milliseconds(3000);

    endwin();
    return 0;
}
