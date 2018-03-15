#include "menu.hpp"
#include "about.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "game.hpp"
#include "high-io.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "physics.hpp"
#include "platform.hpp"
#include "random.hpp"
#include "record.hpp"
#include "settings.hpp"
#include "text.hpp"
#include "version.hpp"
#include <SDL.h>
#include <array>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

class Menu {
public:
  std::string title;
  std::vector<std::string> options;
  size_t selected_option{};
};

/**
 * Writes the provided Menu for the user.
 */
void write_menu(const Menu &menu, SDL_Renderer *renderer) {
  std::vector<std::string> string_vector;
  string_vector.emplace_back(game_name);
  string_vector.insert(std::end(string_vector), std::begin(menu.options), std::end(menu.options));
  const auto selected_option_index = menu.selected_option + 1;
  string_vector[selected_option_index] = "> " + string_vector[selected_option_index] + " <";
  print_menu(string_vector, renderer);
}

Code game(SDL_Renderer *renderer, CommandTable *table) {
  char name[MAXIMUM_PLAYER_NAME_SIZE];
  Player player{};
  Game game{};
  Code code;
  code = read_player_name(name, MAXIMUM_PLAYER_NAME_SIZE, renderer);
  if (code == CODE_QUIT || code == CODE_CLOSE) {
    return code;
  }
  player = create_player(name, table);
  game = create_game(&player);
  code = run_game(&game, renderer);
  destroy_game(&game);
  return code;
}

int main_menu(SDL_Renderer *renderer) {
  int should_quit = 0;
  Code code = CODE_OK;
  Menu menu;
  CommandTable command_table{};
  std::vector<std::string> options = {"Play", "Top Scores", "Info", "Quit"};
  const std::string game_name_string(game_name);
  std::string title = game_name_string + " " + WALLS_OF_DOOM_VERSION;
  menu.title = title;
  menu.options = options;
  menu.selected_option = 0;
  initialize_command_table(&command_table);
  while (should_quit == 0) {
    write_menu(menu, renderer);
    read_commands(&command_table);
    if (static_cast<int>(test_command_table(&command_table, COMMAND_UP, REPETITION_DELAY)) != 0) {
      if (menu.selected_option > 0) {
        menu.selected_option--;
      } else {
        menu.selected_option = menu.options.size() - 1;
      }
    } else if (static_cast<int>(test_command_table(&command_table, COMMAND_DOWN, REPETITION_DELAY)) != 0) {
      if (menu.selected_option + 1 < menu.options.size()) {
        menu.selected_option++;
      } else {
        menu.selected_option = 0;
      }
    } else if ((static_cast<int>(test_command_table(&command_table, COMMAND_ENTER, REPETITION_DELAY)) != 0) ||
               (static_cast<int>(test_command_table(&command_table, COMMAND_CENTER, REPETITION_DELAY)) != 0)) {
      if (menu.selected_option == 0) {
        code = game(renderer, &command_table);
      } else if (menu.selected_option == 1) {
        code = top_scores(renderer, &command_table);
      } else if (menu.selected_option == 2) {
        code = info(renderer, &command_table);
      } else if (menu.selected_option == 3) {
        should_quit = 1;
      }
      /* If it is not defined whether or not we should quit, check the code. */
      if (should_quit == 0) {
        should_quit = is_termination_code(code);
      }
    }
    /* Quit if the user selected the Quit option or closed the window. */
    should_quit =
        static_cast<int>((should_quit != 0) ||
                         static_cast<int>(test_command_table(&command_table, COMMAND_QUIT, REPETITION_DELAY)) != 0);
  }
  return 0;
}
