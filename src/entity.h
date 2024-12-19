// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// entity.h - Generic object that exists in the game
#pragma once

#include "image.h"
#include "mat.h"
#include "monster_sprite.h"

typedef struct Entity Entity;
struct Entity {
    // Every entity is a oriented image
    m4 mtx;
    v2 size;
    Image *img;

    // Monster
    bool is_monster;
    v3 pos;
    v3 pos_old;
    v3 vel;

    u32 health; // Integer health is more satisfying

    Monster_Sprite sprite;
    f32 look_around_timer;

    f32 wiggle_phase;
    f32 wiggle_amp;
    Image *shadow;
    f32 death_animation; // 0 -> alive, 1 -> dead

    // Monster AI
    f32 move_time;
    v2 move_dir;
    v3 look_dir;

    // Next entity in the list
    Entity *next;
};

static Entity *wall_new(Memory *mem, m4 mtx, Image *img) {
    Entity *ent = mem_struct(mem, Entity);
    ent->mtx = mtx;
    ent->img = img;
    return ent;
}
