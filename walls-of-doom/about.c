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
  Code code = read_characters(ABOUT_PAGE_PATH, buffer, ABOUT_PAGE_BUFFER_SIZE);
  if (code != CODE_OK) {
    log_message("Failed to read the text");
    return code;
  } else {
    print_long_text(buffer, renderer);
    /*
     * The code for this function is the code of waiting for input.
     */
    return wait_for_input();
  }
}
