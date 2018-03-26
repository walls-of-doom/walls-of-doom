#include "io.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "menu.hpp"
#include "random.hpp"
#include "text.hpp"
#include "version.hpp"
#include <SDL.h>
#include <cstdio>

enum ParserResult { PARSER_RESULT_CONTINUE, PARSER_RESULT_QUIT };

void log_unrecognized_argument(const std::string &argument) {
  const auto string = "Unrecognized argument: " + argument + ".";
  log_message(string.c_str());
}

ParserResult parse_argument(const char *argument) {
  if (string_equals(argument, "--version")) {
    printf("%s\n", WALLS_OF_DOOM_VERSION);
    return PARSER_RESULT_QUIT;
  }
  log_unrecognized_argument(argument);
  return PARSER_RESULT_QUIT;
}

/* Must be declared with parameters because of SDL 2. */
int main(int argc, char *argv[]) {
  int i;
  int quit = 0;
  int result = 0;
  SDL_Window *window;
  SDL_Renderer *renderer;
  if (argc > 1) {
    for (i = 1; i < argc && (quit == 0); i++) {
      if (parse_argument(argv[i]) == PARSER_RESULT_QUIT) {
        quit = 1;
      }
    }
  }
  if (quit != 0) {
    return result;
  }
  seed_random();
  initialize(&window, &renderer);
  result = main_menu(renderer);
  finalize(&window, &renderer);
  return result;
}
