#pragma once
#include "gfx/box.h"
#include "gfx/image.h"
#include "lib/mat.h"

typedef enum {
    Entity_Player,
    Entity_Wall,
    Entity_Monster,
} Entity_Type;

typedef struct World_Object World_Object;
struct World_Object {
    Entity_Type type;
    Box box;
    void *entity;
    World_Object *next;
};

typedef struct World World;

// Start a new frame
static void world_begin(World *world);

// Incrementally add objects to the world
static void world_add(World *world, Entity_Type type, Box box, void *entity);

// Perform a raycast on the world (previous frame)
static World_Object *world_cast(World *world, v3 pos, v3 dir);

// Find colliding boxes
static World_Object *world_collide(World *world, Memory *mem, Box box);

struct World {
    Memory *mem_next;
    Memory *mem_prev;
    World_Object *obj_next;
    World_Object *obj_prev;
};

static void world_begin(World *world) {
    // Free(prev)
    if (world->mem_prev) mem_free(world->mem_prev);

    // Next -> Prev
    world->mem_prev = world->mem_next;
    world->obj_prev = world->obj_next;

    // next = new memory
    world->mem_next = mem_new();
}

static void world_add(World *world, Entity_Type type, Box box, void *entity) {
    World_Object *obj = mem_struct(world->mem_next, World_Object);
    obj->type = type;
    obj->box = box;
    obj->entity = entity;
    obj->next = world->obj_next;
    world->obj_next = obj;
}

// Find colliding boxes
static World_Object *world_collide(World *world, Memory *mem, Box box) {
    World_Object *ret_list = 0;
    for (World_Object *obj = world->obj_prev; obj; obj = obj->next) {
        if (!box_intersect(obj->box, box)) continue;

        World_Object *ret = mem_struct_uninit(mem, World_Object);
        *ret = *obj;
        ret->next = ret_list;
        ret_list = ret;
    }
    return ret_list;
}
