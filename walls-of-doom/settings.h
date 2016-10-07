#ifndef SETTINGS_H
#define SETTINGS_H

typedef enum RepositionAlgorithm {
  REPOSITION_RANDOM_EMPTY_LINE,
  REPOSITION_RANDOM_WELL_DISTRIBUTED
} RepositionAlgorithm;

RepositionAlgorithm get_reposition_algorithm(void);

#endif
