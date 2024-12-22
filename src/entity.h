// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// entity.h - Generic object that exists in the game
#pragma once
#include "image.h"
#include "mat.h"
#include "monster_sprite.h"

typedef struct Entity Entity;
struct Entity {
    m4 mtx;
    m4 head_mtx;
    v2 size;

    // Visible entity
    Image *img;
    Image *shadow;

    // Moving Entity
    bool can_move;
    bool can_fly;
    bool on_ground;
    v3 pos;
    v3 pos_old;
    v3 vel;

    // Living Entity
    u32 health; // Integer health is more satisfying

    // Monster
    bool is_monster;
    Monster_Sprite sprite;
    f32 look_around_timer;
    f32 wiggle_phase;
    f32 wiggle_amp;
    f32 death_animation; // 0 -> alive, 1 -> dead

    // Monster AI
    f32 move_time;
    v2 move_dir;
    v3 look_dir;

    // Player
    bool is_player;

    // Player rotation around each axis. [0-1]
    v3 rot;
    f32 shoot_time;
    f32 step_volume;

    // Next entity in the list
    Entity *next;
};

static Entity *wall_new(Memory *mem, m4 mtx, Image *img) {
    Entity *ent = mem_struct(mem, Entity);
    ent->mtx = mtx;
    ent->img = img;
    return ent;
}
