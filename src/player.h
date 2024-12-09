// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// player.h - The player character
#pragma once

#include "input.h"
#include "mat.h"
#include "math.h"

typedef struct {
    v3 pos;
    v3 old_pos;

    // Health, probably more satisfying if it is 'discrete' instead of a float
    u32 health;

    // Player rotation around each axis. [0-1]
    v3 rot;
    bool flying;
    bool on_ground;

    // Derived matrices
    m4 body_mtx;
    m4 head_mtx;

    // stuff
    f32 shoot_time;
    f32 step_volume;
} Player;

typedef struct {
    v3 move;
    v3 look;
    bool jump;
    bool fly;
    bool shoot;
    bool shoot_single;
} Player_Input;

// Map Keys and mouse movement to player input
static Player_Input player_parse_input(Input *input) {
    Player_Input in = {};
    if (key_down(input, KEY_W)) in.move.z += 1;
    if (key_down(input, KEY_S)) in.move.z -= 1;
    if (key_down(input, KEY_A)) in.move.x -= 1;
    if (key_down(input, KEY_D)) in.move.x += 1;
    if (key_down(input, KEY_1)) in.look.z += 1.0f / 8;
    if (key_down(input, KEY_2)) in.look.z -= 1.0f / 8;
    if (key_down(input, KEY_SPACE)) in.jump = 1;
    if (key_down(input, KEY_MOUSE_LEFT)) in.shoot = 1;
    if (key_click(input, KEY_MOUSE_LEFT)) in.shoot_single = 1;
    if (key_down(input, KEY_SPACE)) in.move.y += 1;
    if (key_down(input, KEY_SHIFT)) in.move.y -= 1;
    if (key_click(input, KEY_3)) in.fly = 1;

    if (input->mouse_is_grabbed) {
        in.look.y += (f32)input->mouse_rel.x / 1000.0f;
        in.look.x -= (f32)input->mouse_rel.y / 1000.0f;
    }
    return in;
}
