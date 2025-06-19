// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// wall.h - A Quad in 3d space
#pragma once
#include "qfn/collision.h"
#include "qfn/engine.h"
#include "qfn/image.h"
#include "qfn/mat.h"
#include "qfn/monster_sprite.h"

typedef struct Wall Wall;
struct Wall {
    Wall *next;
    m4 mtx;
    Image *image;
};

static Wall *wall2_new(Memory *mem, m4 mtx, Image *image) {
    Wall *wall = mem_struct(mem, Wall);

    m4 mtx2 = m4_id();
    m4_scale_image(&mtx2, image);
    m4_apply(&mtx2, mtx);

    wall->mtx = mtx2;
    wall->image = image_copy(mem, image);
    return wall;
}

static void wall2_update(Wall *wall, Engine *eng, Collision_World *world) {
    gfx_quad_3d(eng->gfx, wall->mtx, wall->image);
    collision_add(world, wall->mtx, wall->image, 0, wall);
}
