#include "io.h"
#include "logger.h"
#include "menu.h"
#include "random.h"
#include <SDL.h>

/* Must be declared with parameters because of SDL 2. */
int main(int argc, char *argv[]) {
  int i;
  int result;
  SDL_Window *window;
  SDL_Renderer *renderer;
  seed_random();
  initialize(&window, &renderer);
  if (argc > 1) {
    log_message("Ignoring the following command line arguments:");
    for (i = 1; i < argc; i++) {
      log_message(argv[i]);
    }
  }
  result = main_menu(renderer);
  finalize(&window, &renderer);
  return result;
}
