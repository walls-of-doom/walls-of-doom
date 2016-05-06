#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "io.h"
#include "platform.h"
#include "random.h"
#include "rest.h"

#define GAME_NAME "Walls of Doom"
#define TOP_BAR_STRING_COUNT 4

#define PLATFORM_COUNT 16

#define MAXIMUM_STRING_SIZE 256

#define PLAYER_SYMBOL "@"
#define GAME_FPS 5

#define MENU_FPS 30

enum COLOR_SCHEME {
    // Color pair 0 is assumed to be white on black, but is actually whatever
    // the terminal implements before color is initialized. It cannot be
    // modified by the application, therefore we must start at 1.
    TOP_BAR_COLOR = 1, // Set the first enum constant to one
    BOTTOM_BAR_COLOR // Becomes two, and so on
};

int initialize_color_schemes(void) {
    init_pair(TOP_BAR_COLOR, COLOR_MAGENTA, COLOR_CYAN);
    init_pair(BOTTOM_BAR_COLOR, COLOR_BLACK, COLOR_YELLOW);
    return 0;
}

typedef struct Player {
    char *name;
    // Could let the color of the player lay here, but this is only
    // advantageous if we have multiple players.
    int x;
    int y;
    unsigned int lives;
    unsigned int score;
} Player;

Player make_player(char *name) {
    Player player;
    player.name = name;
    // Initialize the player to the corner so that it is in a valid state.
    player.x = 0;
    player.y = 0;
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
    mvprintw(0, 0, final_buffer);
    attroff(COLOR_PAIR(TOP_BAR_COLOR));

    return 0;
}

int erase_background(void) {
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);
    int i;
    for (i = 1; i < LINES - 1; i++) {
        mvprintw(i, 0, final_buffer);
    }
    return 0;
}

int write_player(const Player * const player) {
    print(player->x, player->y, PLAYER_SYMBOL);
    return 0;
}

int write_platforms(const Platform * platforms, const size_t platform_count) {
    size_t i;
    for (i = 0; i < platform_count; i++) {
        size_t w;
        for (w = 0; w < platforms[i].width; w++) {
            print(platforms[i].x + w, platforms[i].y, "X");
        }
    }
    return 0;
}

int write_bottom_bar(void) {
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);
    attron(COLOR_PAIR(BOTTOM_BAR_COLOR));
    mvprintw(LINES - 1, 0, final_buffer);
    attroff(COLOR_PAIR(BOTTOM_BAR_COLOR));
    return 0;
}

int update_screen(const Player * const player, const Platform *platforms, const size_t platform_count) {
    write_top_bar(player);
    erase_background();
    write_player(player);
    write_platforms(platforms, platform_count);
    write_bottom_bar();
    refresh();
    return 0;
}

void place_player_on_screen(Player * const player) {
    // This is correct. Even if we don't have the first and the last line, the
    // middle line is unchanged.
    player->x = COLS / 2;
    player->y = LINES / 2;
}

/**
 * The Command enumerated type represents the different commands the user may issue.
 */
typedef enum Command {
    NO_COMMAND,
    COMMAND_UP,
    COMMAND_LEFT,
    COMMAND_CENTER,
    COMMAND_RIGHT,
    COMMAND_DOWN,
    COMMAND_JUMP,
    COMMAND_ENTER
} Command;

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
Command read_next_command() {
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
Command wait_for_next_command() {
    Command command = NO_COMMAND;
    while (command == NO_COMMAND) {
        rest_for_second_fraction(MENU_FPS);
        command = read_next_command();
    }
    return command;
}

/**
 * Initializes the required resources.
 *
 * Should only be called once.
 */
void init(void) {
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
    endwin();
}

void render_menu_label(char *label, int is_selected, int pos, int total) {
    const int starting_y = (LINES - 3 * total) / 2;
    const int y = starting_y + 3 * pos;
    if (is_selected) {
        char buffer[MAXIMUM_STRING_SIZE];
        sprintf(buffer, "> %s <", label);
        label = buffer;
    }
    const int x = (COLS - strlen(label)) / 2;
    print(x, y, label);
}

int render_menu(char **labels, const size_t label_count, const size_t selection) {
    clear();
    size_t i;
    for (i = 0; i < label_count; i++) {
        render_menu_label(labels[i], i == selection, i, label_count);
    }
    return 0;
}

int game(void) {
    Player player = make_player("Player");
    player.x = COLS / 2;
    player.y = LINES / 2;

    Platform platforms[PLATFORM_COUNT];
    size_t i;
    for (i = 0; i < PLATFORM_COUNT; i++) {
        platforms[i].x = random_integer(0, COLS);
        platforms[i].y = random_integer(4, LINES - 4);
        platforms[i].width = random_integer(4, 16);
        platforms[i].speed = random_integer(-3, 3);
    }

    Command command = NO_COMMAND;
    while (1) {
        update_screen(&player, platforms, PLATFORM_COUNT);
        rest_for_second_fraction(GAME_FPS);
        command = read_next_command();
        if (command == COMMAND_LEFT) {
            player.x--;
        } else if (command == COMMAND_RIGHT) {
            player.x++;
        } else if (command == COMMAND_JUMP) {
            player.y--;
        }
    }
    return 0;
}

int menu(void) {
    int got_quit = 0;
    char *options[] = {"Play", "Highscores", "Quit"};
    size_t option_count = 3;
    size_t selection = 0;
    while (!got_quit) {
        render_menu(options, option_count, selection);
        Command command = wait_for_next_command();
        if (command == COMMAND_UP) {
            if (selection > 0) {
                selection--;
            }
        } else if (command == COMMAND_DOWN) {
            if (selection + 1 < option_count) {
                selection++;
            }
        } else if (command == COMMAND_ENTER || command == COMMAND_CENTER) {
            if (selection == 0) {
                game();
            } else if (selection == 2) {
                got_quit = 1;
            }
        }
    }
    return 0;
}

int main(void) {
    init();
    int result = menu();
    finalize();
    return result;
}
