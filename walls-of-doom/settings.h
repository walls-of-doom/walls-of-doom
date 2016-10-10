#ifndef SETTINGS_H
#define SETTINGS_H

#define MAXIMUM_PLATFORM_COUNT 64

typedef enum RepositionAlgorithm {
  REPOSITION_SELECT_BLINDLY,
  REPOSITION_SELECT_AWARELY
} RepositionAlgorithm;

void initialize_settings(void);

RepositionAlgorithm get_reposition_algorithm(void);

long get_platform_count(void);

#endif
