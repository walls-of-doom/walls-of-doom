#include "menu.hpp"
#include "about.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "game.hpp"
#include "io.hpp"
#include "logger.hpp"
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

static const char *profiler_filename = "performance.csv";

class Menu {
public:
  std::string title;
  std::vector<std::string> options;
  size_t selected_option{};
};

/**
 * Writes the provided Menu for the user.
 */
void write_menu(const Settings &settings, const Menu &menu, SDL_Renderer *renderer) {
  std::vector<std::string> string_vector;
  string_vector.emplace_back(game_name);
  string_vector.insert(std::end(string_vector), std::begin(menu.options), std::end(menu.options));
  const auto selected_option_index = menu.selected_option + 1;
  string_vector[selected_option_index] = "> " + string_vector[selected_option_index] + " <";
  print_menu(settings, string_vector, renderer);
}

Code game(const Settings &settings, Profiler *profiler, SDL_Renderer *renderer, CommandTable *table) {
  std::string name;
  Code code = read_player_name(settings, name, renderer);
  if (code == CODE_QUIT || code == CODE_CLOSE) {
    return code;
  }
  Player player(name, table);
  Game game(&player, &settings, profiler);
  return run_game(&game, renderer);
}

int main_menu(const Settings &settings, SDL_Renderer *renderer) {
  auto should_quit = false;
  Code code = CODE_OK;
  Menu menu;
  CommandTable command_table{};
  initialize_command_table(&command_table);
  std::vector<std::string> options = {"Play", "Top Scores", "Info", "Quit"};
  const std::string game_name_string(game_name);
  std::string title = game_name_string + " " + WALLS_OF_DOOM_VERSION;
  menu.title = title;
  menu.options = options;
  menu.selected_option = 0;
  Profiler profiler(true);
  while (!should_quit) {
    write_menu(settings, menu, renderer);
    read_commands(settings, &command_table);
    const auto got_up = test_command_table(&command_table, COMMAND_UP, REPETITION_DELAY);
    const auto got_down = test_command_table(&command_table, COMMAND_DOWN, REPETITION_DELAY);
    const auto got_enter = test_command_table(&command_table, COMMAND_ENTER, REPETITION_DELAY);
    const auto got_center = test_command_table(&command_table, COMMAND_CENTER, REPETITION_DELAY);
    if (got_up) {
      if (menu.selected_option > 0) {
        menu.selected_option--;
      } else {
        menu.selected_option = menu.options.size() - 1;
      }
    } else if (got_down) {
      if (menu.selected_option + 1 < menu.options.size()) {
        menu.selected_option++;
      } else {
        menu.selected_option = 0;
      }
    } else if (got_enter || got_center) {
      if (menu.selected_option == 0) {
        code = game(settings, &profiler, renderer, &command_table);
      } else if (menu.selected_option == 1) {
        code = top_scores(settings, profiler, renderer, &command_table);
      } else if (menu.selected_option == 2) {
        code = info(settings, renderer, &command_table);
      } else if (menu.selected_option == 3) {
        should_quit = true;
      }
      if (!should_quit) {
        should_quit = is_termination_code(code);
      }
    }
    /* Quit if the user selected the Quit option or closed the window. */
    should_quit = should_quit || test_command_table(&command_table, COMMAND_QUIT, REPETITION_DELAY);
  }
  auto full_path = get_full_path(profiler_filename);
  write_string(full_path.c_str(), profiler.dump());
  return 0;
}
