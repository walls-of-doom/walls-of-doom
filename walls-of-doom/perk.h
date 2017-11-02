#ifndef PERK_H
#define PERK_H

#define PERK_INTERVAL_IN_SECONDS 30
#define PERK_INTERVAL_IN_FRAMES (PERK_INTERVAL_IN_SECONDS * FPS)

#define PERK_SCREEN_DURATION_IN_SECONDS 15
#define PERK_PLAYER_DURATION_IN_SECONDS 10
#define PERK_SCREEN_DURATION_IN_FRAMES (PERK_SCREEN_DURATION_IN_SECONDS * FPS)
#define PERK_PLAYER_DURATION_IN_FRAMES (PERK_PLAYER_DURATION_IN_SECONDS * FPS)

typedef enum Perk {
  PERK_POWER_INVINCIBILITY,
  PERK_POWER_LEVITATION,
  PERK_POWER_LOW_GRAVITY,
  PERK_POWER_SUPER_JUMP,
  PERK_POWER_TIME_STOP,
  PERK_CURSE_ACCELERATE_PLATFORMS,
  PERK_CURSE_REVERSE_PLATFORMS,
  PERK_BONUS_EXTRA_POINTS,
  PERK_BONUS_EXTRA_LIFE,
  PERK_COUNT,
  PERK_NONE
} Perk;

Perk get_random_perk(void);

int is_bonus_perk(const Perk perk);

int is_curse_perk(const Perk perk);

/**
 * Returns the name of the provided Perk.
 *
 * The biggest string this function returns has 13 printable characters.
 */
char *get_perk_name(Perk perk);

#endif
