#ifndef RECORD_H
#define RECORD_H

#include "code.h"
#include "constants.h"
#include "player.h"
#include <SDL.h>
#include <stdlib.h>

/**
 * A Record is a structure that stores the outcome of a game.
 */
typedef struct Record {
  char name[MAXIMUM_PLAYER_NAME_SIZE];
  long score;
} Record;

/**
 * Creates a new Record from the provided data.
 *
 * If the name is too big to fit into the array, it is truncated to the maximum
 * possible size.
 */
Record make_record(const char *name, const int score);

/**
 * Writes the specified Record to to the system.
 *
 * Returns the index of the provided record in the record table or -1 if it
 * didn't make it to the table.
 */
int save_record(const Record *const record);

/**
 * Reads Records from the system into the specified destination. Reading at
 * most destination_size Records.
 *
 * Returns how many Records were actually read.
 */
size_t read_records(Record *destination, size_t destination_size);

/**
 * Loads and presents the top scores on the screen.
 */
Code top_scores(SDL_Renderer *renderer);

#endif
