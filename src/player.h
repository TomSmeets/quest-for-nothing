// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// player.h - The player character
#pragma once

#include "mat.h"
#include "math.h"

typedef struct {
    v3 pos;

    // Player rotation around each axis. [0-1]
    v3 rot;

    // Health, probably more satisfying if it is 'discrete' instead of a float
    u32 health;
} Player;


static void player_jump(Player *pl) {
}

static void player_look(Player *pl, f32 yaw, f32 pitch) {
    pl->rot.x += pitch;
    pl->rot.y += yaw;

    pl->rot.x = f_clamp(pl->rot.x, -0.5, 0.5);
    while(pl->rot.y < -1) pl->rot.y += 2;
    while(pl->rot.y >  1) pl->rot.y -= 2;
}

static void player_update(void) {

}
