#ifndef RECORD_H
#define RECORD_H

#include "code.hpp"
#include "constants.hpp"
#include "player.hpp"
#include <SDL.h>
#include <stdlib.h>
#include <string>

/**
 * A Record is a structure that stores the outcome of a game.
 */
class Record {
public:
  char name[MAXIMUM_PLAYER_NAME_SIZE];
  long score;
};

/**
 * Creates a new Record from the provided data.
 *
 * If the name is too big to fit into the array, it is truncated to the maximum possible size.
 */
Record make_record(const char *name, Score score);

/**
 * Writes the specified Record to to the system.
 *
 * Returns the index of the provided record in the record table or -1 if it didn't make it to the table.
 */
int save_record(Record *record);

/**
 * Reads Records from the system into the specified destination. Reading at most destination_size Records.
 *
 * Returns how many Records were actually read.
 */
size_t read_records(Record *destination, size_t destination_size);

Code top_scores(SDL_Renderer *renderer, CommandTable *table);

#endif
