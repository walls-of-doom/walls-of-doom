#ifndef SETTINGS_H
#define SETTINGS_H

/* These maximums are made public so that static allocation is possible. */

#define MAXIMUM_PLATFORM_COUNT 64
#define MAXIMUM_COLUMNS 120
#define MAXIMUM_LINES 40

typedef enum RepositionAlgorithm {
  REPOSITION_SELECT_BLINDLY,
  REPOSITION_SELECT_AWARELY
} RepositionAlgorithm;

void initialize_settings(void);

RepositionAlgorithm get_reposition_algorithm(void);

long get_platform_count(void);

long get_columns(void);

long get_lines(void);

#endif
