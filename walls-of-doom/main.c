#include "high-io.h"
#include "logger.h"
#include "memory.h"
#include "menu.h"
#include "random.h"
#include "text.h"
#include "version.h"
#include <SDL.h>
#include <stdio.h>

typedef enum ParserResult { PARSER_RESULT_CONTINUE, PARSER_RESULT_QUIT } ParserResult;

void log_unrecognized_argument(const char *argument) {
  const size_t start_size = 128;
  const size_t input_size = strlen(argument);
  const size_t string_size = start_size + input_size + 1;
  char *string = NULL;
  string = resize_memory(string, string_size);
  sprintf(string, "Unrecognized argument: %s.", argument);
  log_message(string);
  resize_memory(string, 0);
}

ParserResult parse_argument(const char *argument) {
  if (string_equals(argument, "--version")) {
    printf("%s\n", WALLS_OF_DOOM_VERSION);
    return PARSER_RESULT_QUIT;
  } else {
    log_unrecognized_argument(argument);
    return PARSER_RESULT_QUIT;
  }
}

/* Must be declared with parameters because of SDL 2. */
int main(int argc, char *argv[]) {
  int i;
  int quit = 0;
  int result = 0;
  SDL_Window *window;
  SDL_Renderer *renderer;
  if (argc > 1) {
    for (i = 1; i < argc && !quit; i++) {
      if (parse_argument(argv[i]) == PARSER_RESULT_QUIT) {
        quit = 1;
      }
    }
  }
  if (quit) {
    return result;
  }
  seed_random();
  initialize(&window, &renderer);
  result = main_menu(renderer);
  finalize(&window, &renderer);
  return result;
}
