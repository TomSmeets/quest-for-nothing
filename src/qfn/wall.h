// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// wall.h - A Quad in 3d space
#pragma once
#include "qfn/entity.h"
#include "qfn/image.h"
#include "qfn/mat.h"

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
    wall->image = image;
    return wall;
}
