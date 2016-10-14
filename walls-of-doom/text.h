#ifndef TEXT_H
#define TEXT_H

#include <stdlib.h>

/**
 * Copy from the source string to the destination string, using at most size
 * characters.
 *
 * NUL terminates the destination string, unless size == 0.
 *
 * This function behaves similarly to strlcpy. However, because the latter may
 * not be available on all platforms, copy_string is used.
 */
size_t copy_string(char *destination, const char *source, const size_t size);

/**
 * Copy from the source string to the destination string, up to the specified
 * character, using at most size characters.
 *
 * NUL terminates the destination string, unless size == 0.
 */
size_t copy_string_up_to(char *destination, const char *source, const char *end,
                         const size_t size);

/**
 * Returns a pointer to the start of the text of the string.
 *
 * This is either the first character which is not a space or '\0'.
 */
char *find_start_of_text(char *string);

/**
 * Returns a pointer to the end of the text of the string.
 *
 * This is either the first trailing space or '\0'.
 */
char *find_end_of_text(char *string);

/**
 * Trims a string by removing whitespace from its start and from its end.
 */
void trim_string(char *string);

/**
 * Wraps the input string at the right margin respecting a limit of COLUMNS.
 */
void wrap_at_right_margin(char *string, const size_t columns);

#endif
