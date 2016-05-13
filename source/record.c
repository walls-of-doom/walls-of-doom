#include "record.h"

#include <string.h>

/**
 * Creates a new Record from the provided data.
 *
 * If the name is too big to fit into the array, it is truncated to the maximum
 * possible size.
 */
Record make_record(const char * name, const int score) {
    Record record;
    
    // Safely copy the provided name into the array.
    strncpy(record.name, name, RECORD_NAME_MAXIMUM_SIZE - 1);
    // We must manually assure that the last char is a null character.
    record.name[RECORD_NAME_MAXIMUM_SIZE - 1] = '\0';

    record.score = score;
    
    return record;
}
