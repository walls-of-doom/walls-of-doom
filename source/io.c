#include "constants.h"
#include "io.h"
#include "physics.h"

#include <curses.h>

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

