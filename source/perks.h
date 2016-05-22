#ifndef PERKS_H
#define PERKS_H

typedef enum Perk {
    PERK_POWER_INVINCIBILITY,
    PERK_POWER_LEVITATION,
    PERK_POWER_LOW_GRAVITY,
    PERK_POWER_SUPER_JUMP,
    PERK_POWER_TIME_STOP,
    PERK_BONUS_EXTRA_POINTS,
    PERK_BONUS_EXTRA_LIFE,
    PERK_COUNT,
    PERK_NONE
} Perk;

char get_perk_symbol();

Perk get_random_perk();

# endif
