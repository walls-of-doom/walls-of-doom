#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "constants.h"
#include "io.h"
#include "logger.h"
#include "physics.h"
#include "random.h"
#include "rest.h"
#include "version.h"

#define GAME_NAME "Walls of Doom"
#define TOP_BAR_STRING_COUNT 4

#define PLATFORM_COUNT 8

#define MAXIMUM_STRING_SIZE 256

typedef enum ColorScheme {
    // Color pair 0 is assumed to be white on black, but is actually whatever
    // the terminal implements before color is initialized. It cannot be
    // modified by the application, therefore we must start at 1.
    TOP_BAR_COLOR = 1, // Set the first enum constant to one
    PLATFORM_COLOR,    // Becomes two, and so on
    BOTTOM_BAR_COLOR
} ColorScheme;

int initialize_color_schemes(void) {
    init_pair(TOP_BAR_COLOR, COLOR_CYAN, COLOR_MAGENTA);
    init_pair(PLATFORM_COLOR, COLOR_WHITE, COLOR_WHITE);
    init_pair(BOTTOM_BAR_COLOR, COLOR_BLACK, COLOR_YELLOW);
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

/**
 * Writes the top status bar on the screen for a given Player.
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

int draw_bottom_bar(void) {
    char final_buffer[COLS + 1];
    memset(final_buffer, ' ', COLS);
    attron(COLOR_PAIR(BOTTOM_BAR_COLOR));
    mvprintw(LINES - 1, 0, final_buffer);
    attroff(COLOR_PAIR(BOTTOM_BAR_COLOR));
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

int draw(const Player * const player, const Platform *platforms, const size_t platform_count, const BoundingBox * const box) {
    erase_background();
    draw_top_bar(player);
    draw_bottom_bar();
    draw_platforms(platforms, platform_count, box);
    draw_player(player);
    draw_borders();
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

typedef struct Menu {
    char *title;
    char **options;
    size_t option_count;
    size_t selected_option;
} Menu;

void write_menu(const Menu * const menu) {
    clear();
    const size_t entries = menu->option_count + 1;
    const unsigned int ENTRY_HEIGHT = 3;
    const unsigned int height = entries * ENTRY_HEIGHT;
    const int starting_y = (LINES - height) / 2;
    int y = starting_y + 1;
    print((COLS - strlen(menu->title)) / 2, y, menu->title);
    size_t i;
    for (i = 0; i < menu->option_count; i++) {
        char *string = menu->options[i];
        if (i == menu->selected_option) {
            char buffer[MAXIMUM_STRING_SIZE];
            sprintf(buffer, "> %s <", string);
            string = buffer;
        }
        const int x = (COLS - strlen(string)) / 2;
        y += ENTRY_HEIGHT;
        if (i == menu->selected_option) {
            attron(A_BOLD);
        }
        print(x, y, string);
        if (i == menu->selected_option) {
            attroff(A_BOLD);
        }
    }
}

typedef struct Game {
    Player *player;
    unsigned int score;
    Platform **platforms;
    size_t platform_count;
} Game;

BoundingBox bounding_box_from_screen() {
    BoundingBox box;
    box.min_x = 1;
    box.min_y = 2; // Top bar.
    box.max_x = COLS - 2;
    box.max_y = LINES - 3; // Bottom bar.
    return box;
}

void register_highscore(const Player * const player) {
    char buffer[256];
    sprintf(buffer, "Register a highscore of %d points", player->score);
    log_message(buffer);
    char *message = "Highscores coming soon!";
    const size_t message_size = strlen(message);
    clear();
    print((COLS - message_size) / 2, LINES / 2, message);
    refresh();
    rest_for_nanoseconds(2UL * NANOSECONDS_IN_ONE_SECOND);
}

int check_for_screen_size_change(const BoundingBox * const box) {
    BoundingBox current_box = bounding_box_from_screen();
    return !bounding_box_equals(box, &current_box);
}

int game(void) {
    BoundingBox box = bounding_box_from_screen();

    Player player = make_player("Player");
    player.x = COLS / 2;
    player.y = LINES / 2;

    Platform platforms[PLATFORM_COUNT];
    size_t i;
    for (i = 0; i < PLATFORM_COUNT; i++) {
        platforms[i].x = random_integer(0, COLS);
        platforms[i].y = random_integer(4, LINES - 4);
        platforms[i].width = random_integer(4, 16);
        platforms[i].speed_x = 0;
        platforms[i].speed_y = 0;
        int movement_type = random_integer(0, 4);
        if (movement_type < 2) {
            platforms[i].speed_x = random_integer(1, 3);
        } else if (movement_type < 4) {
            platforms[i].speed_x = random_integer(-3, -1);
        } else {
            platforms[i].speed_y = random_integer(-3, -1);
        }
    }

    unsigned long frame = 0;
    unsigned long next_frame_score = GAME_FPS;
    Command command = NO_COMMAND;
    while (command != COMMAND_QUIT && !check_for_screen_size_change(&box)) {
        // Game loop description
        // 1. Update the score
        if (frame == next_frame_score) {
            player.score++;
            next_frame_score += GAME_FPS;
        }
        // 2. Update the platforms
        update_platforms(&player, platforms, PLATFORM_COUNT, &box);
        // 3. Draw everything
        draw(&player, platforms, PLATFORM_COUNT, &box);
        // 4. Sleep
        rest_for_second_fraction(GAME_FPS);
        // 5. Read whatever command we got (if any)
        command = read_next_command();
        // 6. Update the player using the command (may be a no-op)
        update_player(&player, platforms, PLATFORM_COUNT, &box, command);
        // 7. Increment the frame counter
        frame++;
        if (player.lives == 0) { // <= would be safer but could hide some bugs
            register_highscore(&player);
            break;
        }
    }
    return 0;
}

int main_menu(void) {
    Menu menu;
    char title[MAXIMUM_STRING_SIZE];
    sprintf(title, "%s version %s", "Walls of Doom", WALLS_OF_DOOM_VERSION);
    menu.title = title;
    char *options[] = {"Play", "Highscores", "Quit"};
    menu.options = options;
    menu.option_count = 3;
    menu.selected_option = 0;

    int got_quit = 0;
    while (!got_quit) {
        write_menu(&menu);
        Command command = wait_for_next_command();
        if (command == COMMAND_UP) {
            if (menu.selected_option > 0) {
                menu.selected_option--;
            }
        } else if (command == COMMAND_DOWN) {
            if (menu.selected_option + 1 < menu.option_count) {
                menu.selected_option++;
            }
        } else if (command == COMMAND_ENTER || command == COMMAND_CENTER) {
            if (menu.selected_option == 0) {
                game();
            } else if (menu.selected_option == 2) {
                got_quit = 1;
            }
        }
    }
    return 0;
}

int main(void) {
    init();
    seed_random();
    int result = main_menu();
    finalize();
    return result;
}
