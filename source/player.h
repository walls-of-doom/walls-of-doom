#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_NAME_MAXIMUM_SIZE 64

typedef struct Player {
    char *name;
    // Could let the color of the player lay here, but this is only
    // advantageous if we have multiple players.
    int x;
    int y;
    int speed_x;
    int speed_y;
    // Whether or not the player is being affected by physics.
    int physics;
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
