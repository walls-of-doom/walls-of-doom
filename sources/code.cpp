#include "code.h"

/**
 * Returns whether or not the Code is a termination code.
 */
int is_termination_code(const Code code) { return code == CODE_QUIT || code == CODE_CLOSE; }
