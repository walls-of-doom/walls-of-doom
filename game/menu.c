#include "menu.h"

#include "about.h"
#include "constants.h"
#include "data.h"
#include "game.h"
#include "io.h"
#include "logger.h"
#include "platform.h"
#include "physics.h"
#include "random.h"
#include "record.h"
#include "rest.h"
#include "version.h"

#include <stdlib.h>
#include <string.h>

#include <curses.h>

typedef struct Menu {
    char *title;
    char **options;
    size_t option_count;
    size_t selected_option;
} Menu;

/**
 * Writes the provided Menu for the user.
 */
void write_menu(const Menu * const menu) {
    const size_t entries = menu->option_count + 1;
    const unsigned int ENTRY_HEIGHT = 3;
    const unsigned int height = entries * ENTRY_HEIGHT;
    const int starting_y = (LINES - height) / 2;
    int y = starting_y + 1;
    int x;
    size_t i;
    char buffer[MAXIMUM_STRING_SIZE];
    clear();
    print((COLS - strlen(menu->title)) / 2, y, menu->title);
    for (i = 0; i < menu->option_count; i++) {
        char *string = menu->options[i];
        if (i == menu->selected_option) {
            sprintf(buffer, "> %s <", string);
            string = buffer;
        }
        x = (COLS - strlen(string)) / 2;
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

/**
 * Enters the game.
 */
int game(void) {
    char name[PLAYER_NAME_MAXIMUM_SIZE];
    size_t platform_count;
    Player player;
    Platform platforms[MAXIMUM_PLATFORM_COUNT];
    BoundingBox box;
    Game game;

    read_player_name(name, PLAYER_NAME_MAXIMUM_SIZE);

    player = make_player(name);
    player.x = COLS / 2;
    player.y = LINES / 2;

    box = bounding_box_from_screen();

    platform_count = read_platforms(platforms);

    game = create_game(&player, platforms, platform_count, &box);

    run_game(&game);
    return 0;
}

int main_menu(void) {
    int got_quit = 0;
    Menu menu;
    char title[MAXIMUM_STRING_SIZE];
    char *options[] = {"Play", "Top Scores", "Info", "Quit"};
    Command command;
    sprintf(title, "%s version %s", "Walls of Doom", WALLS_OF_DOOM_VERSION);
    menu.title = title;
    menu.options = options;
    menu.option_count = 4;
    menu.selected_option = 0;

    while (!got_quit) {
        write_menu(&menu);
        command = wait_for_next_command();
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
                top_scores();
            } else if (menu.selected_option == 2) {
                info();
            } else if (menu.selected_option == 3) {
                got_quit = 1;
            }
        }
    }
    return 0;
}
