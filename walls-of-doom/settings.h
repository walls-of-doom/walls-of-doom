#ifndef SETTINGS_H
#define SETTINGS_H

typedef enum RepositionAlgorithm {
  REPOSITION_SELECT_BLINDLY,
  REPOSITION_SELECT_AWARELY
} RepositionAlgorithm;

RepositionAlgorithm get_reposition_algorithm(void);

#endif
