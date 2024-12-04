// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "color.h"
#include "gfx.h"
#include "image.h"
#include "monster_sprite.h"
#include "player.h"
#include "rand.h"
#include "vec.h"

typedef struct Monster {
    // ==== Physics ====
    v3 pos;
    v3 old_pos;

    // ==== AI ====
    u32 health;

    // Current movement direction
    v2 move_dir;

    // Cooldown until new movement direction is chosen
    f32 move_time;

    // ==== Animation ====
    // Movement speed
    f32 speed;
    f32 wiggle;

    // ==== Graphics ====
    Monster_Sprite sprite;

    m4 body_mtx;
    m4 sprite_mtx;

    // ==== Other ====
    struct Monster *next;
} Monster;

static Monster *monster_new(Memory *mem, Random *rng, v3 pos) {
    Monster *mon = mem_struct(mem, Monster);
    mon->pos = pos;
    mon->old_pos = pos;
    mon->health = 10;
    mon->sprite = monster_sprite_generate(mem, rng);
    return mon;
}

static void monster_update(Monster *mon, f32 dt, Player *player, Random *rng, Gfx *gfx) {
    // ==== Physics ====
    v3 vel = mon->pos - mon->old_pos;
    mon->old_pos = mon->pos;

    // Apply movement
    mon->pos.xz += mon->move_dir * dt;

    mon->move_time -= dt;
    if (mon->move_time <= 0) {
        mon->move_time = rand_f32_range(rng, 2, 10);

        u32 mode = rand_u32_range(rng, 0, 2);
        // fmt_su(OS_FMT, "mode=", mode, "\n");
        if (mode == 0) mon->move_dir = 0;
        if (mode == 1) mon->move_dir = v2_normalize(player->pos.xz - mon->pos.xz) * 0.1;
        if (mode == 2) mon->move_dir = v2_from_rot(rand_f32_signed(rng) * PI) * 0.25;
        monster_sprite_update_eyes(&mon->sprite, rng);
    }

    // Player Collision
    v2 player_dir = player->pos.xz - mon->pos.xz;
    f32 player_distance = v2_length(player_dir);

    float radius = 0.5;
    if (player_distance < radius) {
        mon->pos.xz -= player_dir * (radius - player_distance) / player_distance;
    }

    // ==== Animation ====
    f32 speed = v3_length(vel);
    mon->speed += (speed / dt - mon->speed) * dt;
    mon->wiggle += speed * 2;
    mon->wiggle = f_fract(mon->wiggle);
    mon->body_mtx = m4_billboard(mon->pos, player->pos, f_sin2pi(mon->wiggle) * f_min(mon->speed * 0.5, 0.2) * 0.3);

    mon->sprite_mtx = m4_id();
    m4_translate(&mon->sprite_mtx, (v3){0, 0.5, 0});
    m4_scale(&mon->sprite_mtx, (v3){(f32)mon->sprite.image->size.x / 32.0f, (f32)mon->sprite.image->size.y / 32.0f, 1});
    m4_apply(&mon->sprite_mtx, mon->body_mtx);
    gfx_quad_3d(gfx, mon->sprite_mtx, mon->sprite.image);
}
