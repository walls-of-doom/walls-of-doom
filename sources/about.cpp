#include "about.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "high-io.hpp"
#include "logger.hpp"
#include <string.h>

/**
 * Presents information about the game to the player.
 */
Code info(SDL_Renderer *renderer, CommandTable *table) {
  char buffer[ABOUT_PAGE_BUFFER_SIZE];
  Code code = read_characters(ABOUT_PAGE_PATH, buffer, ABOUT_PAGE_BUFFER_SIZE);
  if (code != CODE_OK) {
    log_message("Failed to read the text.");
    return code;
  }
  print_long_text(buffer, renderer);
  return wait_for_input(table);
}
