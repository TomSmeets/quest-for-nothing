// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// wall.h - A Quad in 3d space
#pragma once
#include "mat.h"
#include "collision.h"
#include "engine.h"
#include "image.h"
#include "sparse_set.h"

typedef struct Wall Wall;

struct Wall {
    Wall *next;
    m4 mtx;
    Image *image;
};

static Wall *wall2_new(Memory *mem) {
}

static void wall2_update(Engine *eng, Sparse_Set *sparse, Wall *wall) {
    gfx_quad_3d(eng->gfx, wall->mtx, wall->image);

    Box box = box_from_quad2(wall->mtx);
    sparse_set_add(sparse, box, wall);
}
