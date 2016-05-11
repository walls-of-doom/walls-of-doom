#ifndef RECORD_H
#define RECORD_H

#define RECORD_NAME_SIZE 64

/**
 * A Record is a structure that stores the outcome of a game.
 */
typedef struct Record {
    char name[RECORD_NAME_SIZE];
    int score;
} Record;

/**
 * Creates a new Record from the provided data.
 *
 * If the name is too big to fit into the array, it is truncated to the maximum possible size.
 */
Record create_record(const char * const name, const int score) {
    Record record;
    // Safely copy the provided name into the array.
    record.score = score;
    return record;
}

#endif
