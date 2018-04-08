#ifndef CODE_H
#define CODE_H

/**
 * Codes passed between functions.
 */
enum Code { CODE_OK, CODE_QUIT, CODE_CLOSE, CODE_ERROR };

inline bool is_termination_code(Code code) {
  return code == CODE_QUIT || code == CODE_CLOSE;
}

#endif
