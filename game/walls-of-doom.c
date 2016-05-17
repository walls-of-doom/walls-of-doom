#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "about.h"
#include "constants.h"
#include "data.h"
#include "io.h"
#include "logger.h"
#include "physics.h"
#include "random.h"
#include "record.h"
#include "rest.h"
#include "version.h"

#define ABOUT_PAGE_BUFFER_SIZE 1024
#define ABOUT_PAGE_PATH "assets/about.txt"
#define MAXIMUM_LINE_WIDTH 80

void place_player_on_screen(Player * const player) {
    // This is correct. Even if we don't have the first and the last line, the
    // middle line is unchanged.
    player->x = COLS / 2;
    player->y = LINES / 2;
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

BoundingBox bounding_box_from_screen() {
    BoundingBox box;
    box.min_x = 1;
    box.min_y = 2; // Top bar.
    box.max_x = COLS - 2;
    box.max_y = LINES - 3; // Bottom bar.
    return box;
}

void register_highscore(const Player * const player) {
    // Log that we are registering the highscore
    char buffer[MAXIMUM_STRING_SIZE];
    const char *format = "Started registering a highscore of %d points for %s";
    sprintf(buffer, format, player->score, player->name);
    log_message(buffer);

    // The name has already been entered to make the Player object.
    Record record = make_record(player->name, player->score);

    // Write the Record to disk
    int scoreboard_index = save_record(&record);
    int position = scoreboard_index + 1;

    sprintf(buffer, "Saved the record successfully");
    log_message(buffer);

    char first_line[MAXIMUM_STRING_SIZE];
    sprintf(first_line, "%s died after making %d points.", player->name, player->score);

    char second_line[MAXIMUM_STRING_SIZE];
    if (position > 0) {
        sprintf(second_line, "%s got to position %d!", player->name, position);
    } else {
        sprintf(second_line, "%s didn't make it to the top scores.", player->name);
    }

    clear();
    print_centered(LINES / 2 - 1, first_line);
    print_centered(LINES / 2 + 1, second_line);
    refresh();

    rest_for_nanoseconds(2UL * NANOSECONDS_IN_ONE_SECOND);
}

int check_for_screen_size_change(const BoundingBox * const box) {
    BoundingBox current_box = bounding_box_from_screen();
    return !bounding_box_equals(box, &current_box);
}

int read_platforms(Platform *platforms) {
    const size_t INTEGER_ARRAY_SIZE = 1 + 2 * MAXIMUM_PLATFORM_COUNT;
    int input_integers[INTEGER_ARRAY_SIZE];
    log_message("Started reading platform data");
    const size_t actually_read = read_integers("assets/platforms.txt", input_integers, INTEGER_ARRAY_SIZE);

    char log_message_buffer[256];
    sprintf(log_message_buffer, "Read %lu integers", actually_read);
    log_message(log_message_buffer);

    const size_t platform_count = input_integers[0] < MAXIMUM_PLATFORM_COUNT ? input_integers[0] : MAXIMUM_PLATFORM_COUNT;
    sprintf(log_message_buffer, "Platform count is %lu", platform_count);
    log_message(log_message_buffer);

    size_t i;
    for (i = 0; i < platform_count; i++) {
        Platform *platform = platforms + i;

        platform->width = input_integers[1 + 2 * i];

        platform->x = random_integer(1, COLS - 1);
        platform->y = random_integer(4, LINES - 4);

        platform->speed_x = 0;
        platform->speed_y = 0;
        const int speed = input_integers[1 + 2 * i + 1];
        const int movement_type = random_integer(0, 4);
        if (movement_type < 2) { // 40%
            platform->speed_x = speed;
        } else if (movement_type < 4) { // 40%
            platform->speed_x = -speed;
        } else { // 20 %
            platform->speed_y = -speed;
        }
    }

    return platform_count;
}

int game(void) {
    BoundingBox box = bounding_box_from_screen();

    Player player = make_player("Player");
    player.x = COLS / 2;
    player.y = LINES / 2;

    Platform platforms[MAXIMUM_PLATFORM_COUNT];
    const size_t platform_count = read_platforms(platforms);

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
        update_platforms(&player, platforms, platform_count, &box);
        // 3. Draw everything
        draw(&player, platforms, platform_count, &box);
        // 4. Sleep
        rest_for_second_fraction(GAME_FPS);
        // 5. Read whatever command we got (if any)
        command = read_next_command();
        // 6. Update the player using the command (may be a no-op)
        update_player(&player, platforms, platform_count, &box, command);
        // 7. Increment the frame counter
        frame++;
        if (player.lives == 0) { // <= would be safer but could hide some bugs
            register_highscore(&player);
            break;
        }
    }
    return 0;
}

/**
 * Returns the number of characters used to represent the provided number on
 * base 10.
 */
int count_digits(int number) {
    char buffer[MAXIMUM_STRING_SIZE];
    sprintf(buffer, "%d", number);
    return strlen(buffer);
}

void record_to_string(const Record * const record, char *buffer, const int expected_width) {
    char padding_string[MAXIMUM_STRING_SIZE];
    memset(padding_string, '.', MAXIMUM_STRING_SIZE - 1);
    padding_string[MAXIMUM_STRING_SIZE - 1] = '\0';
    const char format[] = "%s%*.*s%d";
    int padding_length = expected_width - strlen(record->name) - count_digits(record->score);
    sprintf(buffer, format, record->name, padding_length, padding_length, padding_string, record->score);
}

void highscores(void) {
    if (COLS < 16) {
        return;
    }
    const int line_width = COLS - 8;
    Record records[16];
    size_t actually_read_records = read_records(records, 16);
    int y = 2;
    char line[COLS];
    size_t i;
    clear();
    for (i = 0; i < actually_read_records; i++) {
        record_to_string(records + i, line, line_width);
        print_centered(y + i, line);
    }
    refresh();
    rest_for_nanoseconds(2UL * NANOSECONDS_IN_ONE_SECOND);
}

int main_menu(void) {
    Menu menu;
    char title[MAXIMUM_STRING_SIZE];
    sprintf(title, "%s version %s", "Walls of Doom", WALLS_OF_DOOM_VERSION);
    menu.title = title;
    char *options[] = {"Play", "Highscores", "Info", "Quit"};
    menu.options = options;
    menu.option_count = 4;
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
            } else if (menu.selected_option == 1) {
                highscores();
            } else if (menu.selected_option == 2) {
                info();
            } else if (menu.selected_option == 3) {
                got_quit = 1;
            }
        }
    }
    return 0;
}

int main(void) {
    initialize();
    seed_random();
    int result = main_menu();
    finalize();
    return result;
}
