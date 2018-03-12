#ifndef CODE_H
#define CODE_H

/**
 * Codes passed between functions.
 */
typedef enum Code { CODE_OK, CODE_QUIT, CODE_CLOSE, CODE_ERROR } Code;

/**
 * Returns whether or not the Code is a termination code.
 */
int is_termination_code(Code code);

#endif
