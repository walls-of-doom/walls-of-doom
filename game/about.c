#include "about.h"

#include "constants.h"
#include "data.h"
#include "io.h"
#include "rest.h"

#include <string.h>

/**
 * Presents information about the game to the player.
 */
void info(SDL_Renderer *renderer) {
  char buffer[ABOUT_PAGE_BUFFER_SIZE];
  int read_error =
      read_characters(ABOUT_PAGE_PATH, buffer, ABOUT_PAGE_BUFFER_SIZE);
  if (read_error) {
    log_message("Failed to read the text");
  } else {
    print_long_text(buffer, renderer);
    rest_for_seconds(6);
  }
}
