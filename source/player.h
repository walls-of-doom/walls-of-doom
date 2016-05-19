#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_NAME_MAXIMUM_SIZE 64

typedef struct Player {
    char *name;
    // Could let the color of the player lay here, but this is only
    // advantageous if we have multiple players.
    int x;
    int y;
    // How many jumps they player has left.
    unsigned int jumps;
    unsigned int lives;
    unsigned int score;
} Player;

/**
 * Returns a Player object with the provided name.
 */
Player make_player(char *name);

#endif
