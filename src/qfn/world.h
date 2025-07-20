#pragma once
#include "gfx/box.h"
#include "gfx/image.h"
#include "lib/mat.h"

typedef enum {
    Entity_Player,
    Entity_Wall,
    Entity_Monster,
} Entity_Type;

typedef struct Collision_Object Collision_Object;

struct Collision_Object {
    Entity_Type type;
    m4 mtx;
    Image  *img;
    void *entity;
    Collision_Object *next;
};

typedef struct World World;

static void world_begin(World *world, Memory *tmp);
static void world_add(World *world, m4 transform, Image *img, Entity_Type type, void *entity);

static void world_cast(World *world, v3 pos, v3 dir);
