#pragma once

#include "image.h"
#include "mem.h"
#include "vec.h"

typedef struct Packer_Area {
    v2u pos;
    v2u size;
    u32 image;
    struct Packer_Area *next;
} Packer_Area;

typedef struct {
    Memory *mem;
    // Free
    u32 texture_size;
    Packer_Area *levels[12];
    Packer_Area *used[64];
} Packer;

static Packer *packer_new(u32 texture_size) {
    Memory *mem = mem_new();
    Packer *pack = mem_struct(mem, Packer);
    pack->mem = mem;
    pack->texture_size = texture_size;

    // Add first level
    Packer_Area *l0 = mem_struct(mem, Packer_Area);
    l0->size = (v2u){texture_size, texture_size};
    pack->levels[0] = l0;
    return pack;
}

static void packer_free(Packer *pack) {
    mem_free(pack->mem);
}

static u32 packer_level(Packer *pack, v2u size) {
    u32 next_level_size = pack->texture_size;
    for (u32 level = 0; level < array_count(pack->levels); ++level) {
        next_level_size /= 2;
        if (size.x > next_level_size || size.y > next_level_size) {
            return level;
        }
    }

    // Too small, last level
    return array_count(pack->levels) - 1;
}

static Packer_Area *packer_get(Packer *pack, u32 level) {
    // Max level
    if (level >= array_count(pack->levels)) level = array_count(pack->levels) - 1;

    Packer_Area **slot = pack->levels + level;

    // Try current level
    if (*slot) {
        Packer_Area *area = *slot;
        *slot = area->next;
        area->next = 0;
        return area;
    }

    // No texture left?
    if (level == 0) return 0;

    // Try parent level
    Packer_Area *parent = packer_get(pack, level - 1);
    if (!parent) return 0;

    v2u pos = parent->pos;
    v2u size = parent->size / 2;

    Packer_Area *c0 = parent;
    Packer_Area *c1 = mem_struct(pack->mem, Packer_Area);
    Packer_Area *c2 = mem_struct(pack->mem, Packer_Area);
    Packer_Area *c3 = mem_struct(pack->mem, Packer_Area);

    c0->pos = pos + (v2u){0, 0};
    c1->pos = pos + (v2u){size.x, 0};
    c2->pos = pos + (v2u){0, size.y};
    c3->pos = pos + (v2u){size.x, size.y};

    c0->size = size;
    c1->size = size;
    c2->size = size;
    c3->size = size;

    // Insert all except 'c0'
    c0->next = 0;
    c1->next = c2;
    c2->next = c3;
    c3->next = *slot;
    *slot = c1;
    return c0;
}

static Packer_Area *packer_get_cache(Packer *pack, Image *img) {
    Packer_Area **slot = pack->used + (img->id % array_count(pack->used));
    for (Packer_Area *item = *slot; item; item = item->next) {
        if (item->image == img->id) return item;
    }
    return 0;
}

static Packer_Area *packer_get_new(Packer *pack, Image *img) {
    Packer_Area **slot = pack->used + (img->id % array_count(pack->used));

    // Get new area
    u32 level = packer_level(pack, img->size);
    Packer_Area *area = packer_get(pack, level);
    if (!area) return 0;

    // Insert into hash table
    area->image = img->id;
    area->next = *slot;
    *slot = area;
    return area;
}
