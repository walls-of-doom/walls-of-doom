#include "logger.h"
#include "physics.h"
#include "random.h"

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

/**
 * Evaluates whether or not a point is within a Platform.
 */
int is_within_platform(const int x, const int y, const Platform * const platform) {
    return y == platform->y && x >= platform->x && x < platform->x + platform->width;
}

int is_over_platform(const int x, const int y, const Platform * const platform) {
    return is_within_platform(x, y + 1, platform);
}

void move_platform_horizontally(Player * const player, Platform * const platform, const int direction) {
    log_if_not_normalized(direction);
    if (player->y == platform->y) { // Fail fast if the platform is not on the same line
        if (direction == 1) {
            if (player->x == platform->x + platform->width) {
                player->x++;
            }
        } else if (direction == -1) {
            if (player->x == platform->x - 1) {
                player->x--;
            }
        }
    } else if (is_over_platform(player->x, player->y, platform)) {  // If the player is over the platform
        player->x += direction;
    }
    platform->x += direction;
}

void move_platform_vertically(Player * const player, Platform * const platform, const int direction) {
    log_if_not_normalized(direction);
    if (player->x >= platform->x && player->x < platform->x + platform->width) {
        if (direction == 1) {
            if (player->y == platform->y + 1) {
                player->y++;
            }
        } else if (direction == -1) {
            if (player->y == platform->y - 1) {
                player->y--;
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

void update_platforms(Player * const player, Platform *platforms, size_t platform_count, const BoundingBox * const box) {
    size_t i;
    for (i = 0; i < platform_count; i++) {
        update_platform(player, &platforms[i], box);
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

int is_falling(const Player * const player, const Platform *platforms, const size_t platform_count, const BoundingBox * const box) {
    if (box->max_y == player->y) {
        return 0;
    } else {
        size_t i;
        for (i = 0; i < platform_count; i++) {
            if (player->y == platforms[i].y - 1) {
                if (player->x >= platforms[i].x && player->x < platforms[i].x + platforms[i].width) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void update_player(Player * const player, const Platform *platforms, const size_t platform_count, const BoundingBox * const box, const Command command) {
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
    if (is_falling(player, platforms, platform_count, box)) {
        player->y++;
    }
}
