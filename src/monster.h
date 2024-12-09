// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "color.h"
#include "engine.h"
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

    f32 death_ani;

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
