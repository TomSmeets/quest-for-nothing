// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// entity.h - Generic object that exists in the game
#pragma once
#include "collision.h"
#include "image.h"
#include "mat.h"
#include "monster_sprite.h"

typedef enum {
    Entity_None,
    Entity_Player,  // Player
    Entity_Monster, // Ai
    Entity_Bullet,  //
    Entity_Wall,
} Entity_Type;

// An entity represents any object in the 3D world.
// This improves simplicity and flexibility.
// - Monster: AI
// - Player: Input
// - Bullet
// - Wall
//
// This Simplifies storage, collision detection and allows for
typedef struct Entity Entity;
struct Entity {
    // Body Position, orientation and Scaling (TRS Matrix)
    // Used for collision and rendering
    m4 mtx;

    m4 image_mtx;
    // Entity Head Orientation. (TR Matrix)
    // - Gun direction
    // - Camera direction
    m4 head_mtx;

    // Entity Size (width, height)
    v2 size;

    // Main sprite
    Image *image;

    // Shadow
    Image *shadow;

    bool is_player;
    bool is_monster;
    bool is_wall;
    bool is_flying;
    bool is_alive;

    v3 pos;
    v3 pos_old;
    v3 vel;

    bool on_ground;

    // Living Entity
    u32 health; // Integer health is more satisfying

    // Monster
    Monster_Sprite sprite;
    f32 look_around_timer;
    f32 wiggle_phase;
    f32 wiggle_amp;
    f32 death_animation; // 0 -> alive, 1 -> dead

    // Monster AI
    f32 move_time;
    v2 move_dir;
    v3 look_dir;

    // Player rotation around each axis. [0-1]
    v3 rot;
    f32 recoil_animation;
    f32 step_volume;

    // Next entity in the list
    Entity *next;
};

static Entity *wall_new(Memory *mem, m4 mtx, Image *img) {
    Entity *ent = mem_struct(mem, Entity);
    ent->mtx = mtx;
    ent->image = img;
    ent->is_wall = true;
    return ent;
}
