#include "logger.h"
#include "physics.h"
#include "random.h"
#include "vector.h"

#include <math.h>
#include <stdio.h>

void log_if_not_normalized(const int value) {
    if (value < -1 && value > 1) {
        char buffer[256];
        char *format = "Expected a normalized value, but got %d instead\n";
        sprintf(buffer, format, value);
        log_message(buffer);
    }
}

int bounding_box_equals(const BoundingBox * const a, const BoundingBox * const b) {
    return a->min_x == b->min_x
        && a->min_y == b->min_y
        && a->max_x == b->max_x
        && a->max_y == b->max_y;
}

/**
 * Evaluates whether or not a point is within a Platform.
 */
int is_within_platform(const int x, const int y, const Platform * const platform) {
    return y == platform->y && x >= platform->x && x < platform->x + platform->width;
}

int is_over_platform(const int x, const int y, const Platform * const platform) {
    return is_within_platform(x, y + 1, platform);
}

/**
 * Attempts to force the Player to move according to the provided displacement.
 *
 * If the player does not have physics enabled, this is a no-op.
 */
void shove_player(Player * const player, const Vector displacement) {
    if (player->physics) {
        player->x += displacement.x;
        player->y += displacement.y;
    }
}

void move_platform_horizontally(Player * const player, Platform * const platform, const int direction) {
    log_if_not_normalized(direction);
    if (player->y == platform->y) { // Fail fast if the platform is not on the same line
        if (direction == 1) {
            if (player->x == platform->x + platform->width) {
                Vector displacement;
                displacement.x = 1;
                displacement.y = 0;
                shove_player(player, displacement);
            }
        } else if (direction == -1) {
            if (player->x == platform->x - 1) {
                Vector displacement;
                displacement.x = -1;
                displacement.y = 0;
                shove_player(player, displacement);
            }
        }
    } else if (is_over_platform(player->x, player->y, platform)) {  // If the player is over the platform
        Vector displacement;
        displacement.y = 0;
        // We could just assing direction to x, but I think this is better.
        if (direction == 1) {
            displacement.x = 1;
        } else if (direction == -1) {
            displacement.x = -1;
        } else {
            displacement.x = 0;
        }
        shove_player(player, displacement);
    }
    platform->x += direction;
}

void move_platform_vertically(Player * const player, Platform * const platform, const int direction) {
    log_if_not_normalized(direction);
    if (player->x >= platform->x && player->x < platform->x + platform->width) {
        if (direction == 1) {
            if (player->y == platform->y + 1) {
                Vector displacement;
                displacement.x = 0;
                displacement.y = 1;
                shove_player(player, displacement);
            }
        } else if (direction == -1) {
            if (player->y == platform->y - 1) {
                Vector displacement;
                displacement.x = 0;
                displacement.y = -1;
                shove_player(player, displacement);
            }
        }
    }
    platform->y += direction;
}

/**
 * Repositions a Platform in the vicinity of a BoundingBox.
 */
void reposition(Player * const player, Platform * const platform, const BoundingBox * const box) {
    if (platform->x > box->max_x) { // To the right of the box
        platform->x = 1 - platform->width;
        platform->y = random_integer(box->min_y, box->max_y);
    } else if (platform->x + platform->width < box->min_x) { // To the left of the box
        platform->x = box->max_x;
        platform->y = random_integer(box->min_y, box->max_y);
    } else if (platform->y < box->min_y) { // Above the box
        platform->x = random_integer(box->min_x, box->max_x);
        // Must work when the player is in the last line
        platform->y = box->max_y + 1; // Create it under the bounding box
        move_platform_vertically(player, platform, -1); // Use the move function to keep the game in a valid state
    }
    // We don't have to deal with platforms below the box.
}

/**
 * Evaluates whether or not a Platform is completely outside of a BoundingBox.
 *
 * Returns 0 if the platform intersects the bounding box.
 * Returns 1 if the platform is to the left or to the right of the bounding box.
 * Returns 2 if the platform is above or below the bounding box.
 */
int is_out_of_bounding_box(Platform * const platform, const BoundingBox * const box) {
    const int min_x = platform->x;
    const int max_x = platform->x + platform->width;
    if (max_x < box->min_x || min_x > box->max_x) {
        return 1;
    } else if (platform->y < box->min_y || platform->y > box->max_y) {
        return 2;
    } else {
        return 0;
    }
}

void update_platform(Player * const player, Platform * const platform, const BoundingBox * const box) {
    int i;
    for (i = 0; i < abs(platform->speed_x); i++) {
        move_platform_horizontally(player, platform, normalize(platform->speed_x));
    }
    for (i = 0; i < abs(platform->speed_y); i++) {
        move_platform_vertically(player, platform, normalize(platform->speed_y));
    }
    if (is_out_of_bounding_box(platform, box)) {
        reposition(player, platform, box);
    }
}

void update_platforms(Game * const game) {
    size_t i;
    for (i = 0; i < game->platform_count; i++) {
        update_platform(game->player, game->platforms + i, game->box);
    }
}

int is_valid_move(const int x, const int y, const Platform *platforms, const size_t platform_count) {
    size_t i;
    for (i = 0; i < platform_count; i++) {
        if (is_within_platform(x, y, platforms + i)) {
            return 0;
        }
    }
    return 1;
}

/**
 * Evaluates whether or not the Player is falling. Takes the physics field into account.
 */
int is_falling(const Player * const player, const Platform *platforms, const size_t platform_count) {
    if (!player->physics) {
        return 0;
    }
    size_t i;
    for (i = 0; i < platform_count; i++) {
        if (player->y == platforms[i].y - 1) {
            if (player->x >= platforms[i].x && player->x < platforms[i].x + platforms[i].width) {
                return 0;
            }
        }
    }
    return 1;
}

int is_touching_a_wall(const Player * const player, const BoundingBox * const box) {
    return (player->x < box->min_x || player->x > box->max_x) || (player->y < box->min_y || player->y > box->max_y);
}

int get_bounding_box_center_x(const BoundingBox * const box) {
    return box->min_x + (box->max_x - box->min_x + 1) / 2;
}

int get_bounding_box_center_y(const BoundingBox * const box) {
    return box->min_y + (box->max_y - box->min_y + 1) / 2;
}

void reposition_player(Player * const player, const BoundingBox * const box) {
    player->x = get_bounding_box_center_x(box);
    player->y = get_bounding_box_center_y(box);
}

void update_perk(Game * const game) {
    if (game->frame == game->perk_end_frame) { // Current Perk must end
        game->perk = PERK_NONE;
    } else if (game->frame == game->perk_end_frame + GAME_PERK_INTERVAL_IN_FRAMES) {
        game->perk = get_random_perk();
        Vector position;
        position.x = random_integer(game->box->min_x, game->box->max_x);
        position.y = random_integer(game->box->min_y, game->box->max_y);
        game->perk_position = position;
        game->perk_end_frame = game->frame + GAME_PERK_DURATION_IN_FRAMES;
    }
}

void update_player(Game * const game, const Command command) {
    Player * const player = game->player;
    Platform *platforms = game->platforms;
    const size_t platform_count = game->platform_count;
    BoundingBox *box = game->box;
    if (command != COMMAND_NONE) {
        player->physics = 1;
    }
    if (command == COMMAND_LEFT) {
        if (is_valid_move(player->x - 1, player->y, platforms, platform_count)) {
            player->x -= 1;
        }
    } else if (command == COMMAND_RIGHT) {
        if (is_valid_move(player->x + 1, player->y, platforms, platform_count)) {
            player->x += 1;
        }
    }
    // After moving, if it even happened, simulate gravity
    if (is_falling(player, platforms, platform_count)) {
        player->y++;
    }
    if (is_touching_a_wall(player, box)) {
        player->lives--;
        reposition_player(player, box);
        // Unset physics collisions for the player.
        player->physics = 0;
    }
}
