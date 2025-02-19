// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// wall.h - A Quad in 3d space
#pragma once
#include "image.h"
#include "mat.h"
#include "entity.h"

typedef struct Wall Wall;
struct Wall {
    Entity_Type type;
    Wall *next;
    m4 mtx;
    Image *image;
};

static Wall *wall2_new(Memory *mem, m4 mtx, Image *image) {
    Wall *wall = mem_struct(mem, Wall);
    wall->type = Entity_Wall;
    wall->mtx = mtx;
    wall->image = image;
    return wall;
}

static Box wall_box(Wall *wall) {
    return box_from_quad2(wall->mtx);
}
