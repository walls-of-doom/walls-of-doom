#include "game.h"

void update_platform(Platform * const platform, const BoundingBox * const box);
int is_out_of_bounding_box(Platform * const platform, const BoundingBox * const box);
void reposition(Platform * const platform, const BoundingBox * const box);
int is_within_platform(const int x, const int y, const Platform * const platform);
void log_if_amount_is_not_normalized(const int amount);

void update_game(Game * const game) {
    update_platforms(game);
    update_player(game);
}

void update_platforms(Game * const game) {
    size_t i;
    for (i = 0; i < game->platform_count; i++) {
        update_platform(game->player, game->platforms +i, game->box);
    }
}

void update_platform(Platform * const platform, const BoundingBox * const box) {
    platform->x += platform->speed_x;
    platform->y += platform->speed_y;
    if (is_out_of_bounding_box(platform, box)) {
        reposition(platform, box);
    }
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

/**
 * Repositions a Platform in the vicinity of a BoundingBox.
 */
void reposition(Platform * const platform, const BoundingBox * const box) {
    if (platform->x > box->max_x) { // To the right of the box
        platform->x = 1 - platform->width;
        platform->y = random_integer(box->min_y, box->max_y);
    } else if (platform->x + platform->width < box->min_x) { // To the left of the box
        platform->x = box->max_x;
        platform->y = random_integer(box->min_y, box->max_y);
    } else if (platform->y < box->min_y) { // Above the box
        platform->x = random_integer(box->min_x, box->max_x);
        platform->y = box->max_y;
    }
    // We don't have to deal with platforms below the box.
}

void update_player(Game * const game) {
}

/**
 * Evaluates whether or not a point is within a Platform.
 */
int is_within_platform(const int x, const int y, const Platform * const platform) {
    return y == platform->y && x >= platform->x && x < platform->x + platform->width;
}

void log_if_amount_is_not_normalized(const int amount) {
    if (amount < -1 || amount > 1) {
        char buffer[128];
        sprintf(buffer, "Expected a normalized amount but got %d\n", amount);
        log_message(buffer);
    }
}
