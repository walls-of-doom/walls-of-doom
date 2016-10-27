#ifndef SETTINGS_H
#define SETTINGS_H

/* These maximums are made public so that static allocation is possible. */

#define MAXIMUM_PLATFORM_COUNT 256
#define MAXIMUM_COLUMNS 384
#define MAXIMUM_LINES 200

typedef enum RepositionAlgorithm {
  REPOSITION_SELECT_BLINDLY,
  REPOSITION_SELECT_AWARELY
} RepositionAlgorithm;

void initialize_settings(void);

RepositionAlgorithm get_reposition_algorithm(void);

long get_platform_count(void);

int get_font_size(void);

long get_columns(void);

long get_lines(void);

long get_padding(void);

#endif
