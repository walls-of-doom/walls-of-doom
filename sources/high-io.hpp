#ifndef HIGH_IO_H
#define HIGH_IO_H

#include "base-io.hpp"
#include "clock.hpp"
#include "code.hpp"
#include "color.hpp"
#include "command.hpp"
#include "game.hpp"
#include "perk.hpp"
#include "physics.hpp"
#include "record.hpp"
#include <string>
#include <vector>

/**
 * Reads a string from the user of up to size characters (including NUL).
 *
 * The string will be echoed after the prompt, which starts at (x, y).
 */
Code read_string(int x, int y, const char *prompt, char *destination, size_t size, Renderer *renderer);

/**
 * Attempts to read a player name.
 *
 * Returns a Code, which may indicate that the player tried to quit.
 */
Code read_player_name(char *destination, const size_t maximum_size, Renderer *renderer);

/**
 * Draws a full game to the screen.
 *
 * Returns a Milliseconds approximation of the time this function took.
 */
Milliseconds draw_game(const Game *const game, Renderer *renderer);

/**
 * Prints the provided string on the screen starting at (x, y).
 */
Code print(const int x, const int y, const char *string, const ColorPair color_pair, Renderer *renderer);

void print_menu(const std::vector<std::string> &strings, Renderer *renderer);

/**
 * Prints the provided string after formatting it to increase readability.
 */
void print_long_text(char *string, Renderer *renderer);

void print_records(size_t count, const Record *records, Renderer *renderer);

#endif
