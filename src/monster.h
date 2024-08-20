// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "image.h"
#include "rand.h"
#include "vec.h"

typedef struct Monster {
    v3 pos;
    u32 health;
    Image *image;
    struct Monster *next;
} Monster;

static Image *monster_gen_image(Memory *mem, Random *rng) {
    v2u size = {32, 32};

    Image *image = image_new(mem, size);

    float width_inner = 0;
    float width_outer = 4;

    float cfg_spike = 1.0;
    float cfg_overlap = 4.0;

    v4 color_base = rand_color(rng);
    image_fill(image, color_base * (v4){1, 1, 1, 0});

    float size_y = 8 + rand_f32(rng) * (32 - 8);

    for (u32 y = size_y; y < size.y; ++y) {
        for (u32 x = 0; x < size.x; ++x) {
            f32 cx = (f32)x - (f32)size.x / 2.0f + 0.5f;
            if (cx < 0) cx = -cx;

            if (cx < width_outer && cx > width_inner) {
                image->pixels[(size.y - y - 1) * size.x + x] = color_base + rand_color(rng) * 0.05;
            }
        }

        width_outer += rand_f32_signed(rng) * cfg_spike;
        width_inner += rand_f32_signed(rng) * cfg_spike;

        if (width_outer - width_inner < cfg_overlap) {
            width_outer += cfg_overlap / 2;
            width_inner -= cfg_overlap / 2;
        }

        if (width_outer < width_inner) {
            f32 new_width_inner = width_outer;
            width_outer = width_inner;
            width_inner = new_width_inner;
        }
    }
    return image;
}

static Monster *monster_new(Memory *mem, Random *rng, v3 pos) {
    Monster *monster = mem_struct(mem, Monster);
    monster->pos = pos;
    monster->health = 10;
    monster->image = monster_gen_image(mem, rng);
    return monster;
}
