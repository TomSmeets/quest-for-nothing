// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// player.h - The player character
#pragma once
#include "lib/vec.h"
#include "qfn/engine.h"
#include "qfn/entity.h"
#include "qfn/input.h"

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
    if (key_down(input, KEY_A)) in.move.x += 1;
    if (key_down(input, KEY_D)) in.move.x -= 1;

    if (key_down(input, KEY_I)) in.look.x -= 0.015;
    if (key_down(input, KEY_K)) in.look.x += 0.015;
    if (key_down(input, KEY_J)) in.look.y += 0.015;
    if (key_down(input, KEY_L)) in.look.y -= 0.015;

    if (key_down(input, KEY_1)) in.look.z += 1.0f / 8;
    if (key_down(input, KEY_2)) in.look.z -= 1.0f / 8;
    if (key_down(input, KEY_SPACE)) in.jump = 1;
    if (key_down(input, KEY_MOUSE_LEFT)) in.shoot = 1;
    if (key_click(input, KEY_MOUSE_LEFT)) in.shoot_single = 1;
    if (key_down(input, KEY_SPACE)) in.move.y += 1;
    if (key_down(input, KEY_SHIFT)) in.move.y -= 1;

    if (input->mouse_is_grabbed) {
        in.look.y -= (f32)input->mouse_rel.x / 1000.0f;
        in.look.x -= (f32)input->mouse_rel.y / 1000.0f;
    }
    return in;
}

typedef struct {
    v3 pos;
    v3 look;
} Player;

static Player *player2_new(Memory *mem, v3 pos) {
    Player *player = mem_struct(mem, Player);
    player->pos = pos;
    return player;
}

static void player2_update(Player *player, Engine *eng) {
    m4 mtx_body = m4_id();
    m4_rotate_y(&mtx_body, player->look.y);
    m4_translate(&mtx_body, player->pos);

    m4 mtx_head = m4_id();
    m4_rotate_z(&mtx_head, player->look.z);
    m4_rotate_x(&mtx_head, player->look.x);
    m4_translate_y(&mtx_head, 0.5f);
    m4_apply(&mtx_head, mtx_body);

    gfx_debug_mtx(eng->gfx_dbg, mtx_head);
}
