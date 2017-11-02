#include "menu.h"
#include "about.h"
#include "constants.h"
#include "data.h"
#include "game.h"
#include "high-io.h"
#include "logger.h"
#include "memory.h"
#include "physics.h"
#include "platform.h"
#include "random.h"
#include "record.h"
#include "settings.h"
#include "text.h"
#include "version.h"
#include <SDL.h>
#include <stdlib.h>
#include <string.h>

typedef struct Menu {
  char *title;
  char **options;
  size_t option_count;
  size_t selected_option;
} Menu;

/**
 * Writes the provided Menu for the user.
 */
void write_menu(const Menu *const menu, SDL_Renderer *renderer) {
  const size_t entries = menu->option_count + 1;
  const size_t string_count = 2 * entries - 1;
  char **strings = NULL;
  char *source = NULL;
  const char hint_format[] = "> %s <";
  const size_t hint_size = strlen(hint_format) - 2;
  size_t option_index;
  size_t i;
  strings = resize_memory(strings, sizeof(char *) * string_count);
  for (i = 0; i < string_count; i++) {
    strings[i] = NULL;
    strings[i] = resize_memory(strings[i], MAXIMUM_STRING_SIZE);
    if (i == 0) {
      copy_string(strings[i], menu->title, MAXIMUM_STRING_SIZE);
    } else if (i % 2 == 0) {
      /* Note that i == 0 is used for the title. */
      option_index = (i - 2) / 2;
      source = menu->options[option_index];
      if (menu->selected_option == option_index) {
        if (strlen(source) + 2 * hint_size < MAXIMUM_STRING_SIZE) {
          sprintf(strings[i], hint_format, source);
        } else {
          copy_string(strings[i], source, MAXIMUM_STRING_SIZE);
        }
      } else {
        copy_string(strings[i], source, MAXIMUM_STRING_SIZE);
      }
    } else {
      copy_string(strings[i], "", MAXIMUM_STRING_SIZE);
    }
  }
  print_menu(string_count, strings, renderer);
  /* Free the strings. */
  for (i = 0; i < string_count; i++) {
    resize_memory(strings[i], 0);
  }
  /* Free the pointer array. */
  resize_memory(strings, 0);
}

/**
 * Enters the game.
 */
Code game(SDL_Renderer *renderer) {
  char name[MAXIMUM_PLAYER_NAME_SIZE];
  Player player;
  Game game;
  Code code;

  code = read_player_name(name, MAXIMUM_PLAYER_NAME_SIZE, renderer);
  /* If got QUIT or CLOSE, return now. */
  if (code == CODE_QUIT || code == CODE_CLOSE) {
    return code;
  }

  player = create_player(name);

  game = create_game(&player);

  code = run_game(&game, renderer);

  destroy_game(&game);

  return code;
}

int main_menu(SDL_Renderer *renderer) {
  int should_quit = 0;
  Code code = CODE_OK;
  Menu menu;
  char title[MAXIMUM_STRING_SIZE];
  char *options[] = {"Play", "Top Scores", "Info", "Quit"};
  Command command;
  sprintf(title, "%s %s", "Walls of Doom", WALLS_OF_DOOM_VERSION);
  menu.title = title;
  menu.options = options;
  menu.option_count = 4;
  menu.selected_option = 0;

  while (!should_quit) {
    write_menu(&menu, renderer);
    command = wait_for_next_command();
    if (command == COMMAND_UP) {
      if (menu.selected_option > 0) {
        menu.selected_option--;
      } else {
        menu.selected_option = menu.option_count - 1;
      }
    } else if (command == COMMAND_DOWN) {
      if (menu.selected_option + 1 < menu.option_count) {
        menu.selected_option++;
      } else {
        menu.selected_option = 0;
      }
    } else if (command == COMMAND_ENTER || command == COMMAND_CENTER) {
      if (menu.selected_option == 0) {
        code = game(renderer);
      } else if (menu.selected_option == 1) {
        code = top_scores(renderer);
      } else if (menu.selected_option == 2) {
        code = info(renderer);
      } else if (menu.selected_option == 3) {
        should_quit = 1;
      }
      /* If it is not defined whether or not we should quit, check the code. */
      if (should_quit == 0) {
        should_quit = is_termination_code(code);
      }
    }
    /* Quit if the user selected the Quit option or closed the window. */
    should_quit = should_quit || command == COMMAND_QUIT;
  }
  return 0;
}
