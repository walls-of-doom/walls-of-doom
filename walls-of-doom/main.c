#include "io.h"
#include "menu.h"
#include "random.h"
#include <SDL.h>

int main(void) {
  int result;
  SDL_Window *window;
  SDL_Renderer *renderer;
  seed_random();
  initialize(&window, &renderer);
  result = main_menu(renderer);
  finalize(&window, &renderer);
  return result;
}
