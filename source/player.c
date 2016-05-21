#include "player.h"

Player make_player(char *name) {
    Player player;
    player.name = name;
    // Initialize the player to the corner so that it is in a valid state.
    player.x = 0;
    player.y = 0;
    player.physics = 0;
    player.jumps = 2;
    player.lives = 3;
    player.score = 0;
    return player;
}
