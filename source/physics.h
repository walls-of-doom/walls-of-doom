#ifndef PHYSICS_H
#define PHYSICS_H

#include "command.h"
#include "platform.h"
#include "player.h"
#include "stdlib.h"

typedef struct BoundingBox {
    int min_x;
    int min_y;
    int max_x;
    int max_y;
} BoundingBox;

/**
 * Repositions a Platform in the vicinity of a BoundingBox.
 */
void reposition(Platform * const platform, const BoundingBox * const box);

/**
 * Evaluates whether or not a Platform is completely outside of a BoundingBox.
 *
 * Returns 0 if the platform intersects the bounding box.
 * Returns 1 if the platform is to the left or to the right of the bounding box.
 * Returns 2 if the platform is above or below the bounding box.
 */
int is_out_of_bounding_box(Platform * const platform, const BoundingBox * const box);

/**
 * Evaluates whether or not a point is within a Platform.
 */
int is_within_platform(const int x, const int y, const Platform * const platform);

void update_platform(Player * const player, Platform * const platform, const BoundingBox * const box);

void update_platforms(Player * const player, Platform * const platforms, const size_t platform_count, const BoundingBox * const box);

void update_player(Player * const player, const Platform *platforms, const size_t platform_count, const BoundingBox * const box, const Command command);

#endif
