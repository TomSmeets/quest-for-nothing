// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// texture_packer.h - Simple texture packer
#pragma once
#include "lib/mem.h"
#include "lib/vec.h"
#include "qfn/image.h"

// A region in a texture atlas
typedef struct Packer_Area {
    // Start position
    v2u pos;

    // Size in pixels
    v2u size;

    // Image id (new images get a unique id)
    u32 image;

    // Modified images increment variation
    u32 variation;

    // Next pointer for linked list
    struct Packer_Area *next;
} Packer_Area;

typedef struct {
    // Destination memory arena
    Memory *mem;

    // Atlas texture size (width and height, the texture is a square)
    u32 texture_size;

    // Free texture areas, indexed by size
    // Each size gets their own level.
    // level 0 is the biggest size (texture_size x texture_size)
    // level 1 is the halved size, so 4 squares of (texture_size / 2) x (texture_size / 2)
    // level 2 is those halved again...
    // level 11 is the smalles possible bucket size (texture_size / 2048)
    Packer_Area *levels[12];

    // A hashmap of used texture areas.
    // Indexed by the image id for quick lookup.
    Packer_Area *used[64];

    // Statistic that tracks how many pixels are already used
    // (Equal to "sum(area(u.size) for u in used)")
    u32 total_space_used;
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
    area->variation = img->variation;
    area->next = *slot;
    *slot = area;
    return area;
}

// Return the number of pixels remaining in this texture
static u32 packer_capacity(Packer *pack, u32 size) {
    u32 total = 0;
    u32 end_level = packer_level(pack, (v2u){size, size});
    for (u32 level = 0; level <= end_level; ++level) {
        for (Packer_Area *item = pack->levels[level]; item; item = item->next) {
            total += (item->size.x / size) * (item->size.y / size);
        }
    }
    return total;
}
