#include "about.h"

#include "constants.h"
#include "data.h"
#include "io.h"

#include <string.h>

/**
 * Presents information about the game to the player.
 */
Code info(SDL_Renderer *renderer) {
  char buffer[ABOUT_PAGE_BUFFER_SIZE];
  int error = read_characters(ABOUT_PAGE_PATH, buffer, ABOUT_PAGE_BUFFER_SIZE);
  if (error) {
    log_message("Failed to read the text");
    return CODE_ERROR;
  } else {
    print_long_text(buffer, renderer);
    /*
     * The code for this function is the code of waiting for input.
     */
    return wait_for_input();
  }
}
