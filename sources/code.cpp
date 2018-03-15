#include "code.hpp"

/**
 * Returns whether or not the Code is a termination code.
 */
int is_termination_code(const Code code) { return static_cast<int>(code == CODE_QUIT || code == CODE_CLOSE); }
