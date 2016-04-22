#include <curses.h>

#include "rest.h"

#define PLAYER_SYMBOL "@"

int main() {
    // Initialize the screen.
    initscr();
    // Prevent terminal echo.
    noecho();
    // Do not display the cursor.
    curs_set(FALSE);

    mvprintw(0, 0, PLAYER_SYMBOL);

    refresh();

    rest_for_milliseconds(3000);

    endwin();
    return 0;
}
