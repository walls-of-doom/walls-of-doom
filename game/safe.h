#ifndef SAFE_H
#define SAFE_H

/*
 * Copy from the source string to the destination string, using at most size
 * characters.
 *
 * NUL terminates the destination string, unless size == 0.
 *
 * This function behaves similarly to strlcpy. However, because the latter may
 * not be available on all platforms, safe_strcpy is used.
 */
size_t safe_strcpy(char *destination, const char *source, size_t size);

#endif
