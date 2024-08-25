// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "image.h"
#include "rand.h"
#include "vec.h"

typedef struct Monster {
    v3 pos;
    u32 health;
    u32 eye_x;
    u32 eye_y;
    u32 mouth_y;
    u32 look_dir;

    v4 color_base;
    Image *image;
    struct Monster *next;
} Monster;

static void monster_gen_image(Monster *mon, Memory *mem, Random *rng) {

    // Initial line widths
    float cfg_start_width = rand_f32_range(rng, 1.0f, 8.0f);
    float cfg_antenna_x   = rand_f32_range(rng, 1.0f, cfg_start_width);

    // Parameters
    float cfg_spike = 0.5 + rand_f32(rng)*1.5;
    float cfg_overlap = 4.0;

    v4 color_base = rand_color(rng);

    float size_y = rand_f32_range(rng, 8, 32 - 4);

    // Image size
    v2u size = {32, 32};

    // Clear image with base color
    // This improves alpha blending
    // Probably go to premultiplied alpha
    Image *image = image_new(mem, size);
    image_fill(image, color_base * (v4){1, 1, 1, 0});

    u32 offset = size.y - size_y - 1;

    u32 eye_y = offset + rand_u32_range(rng, 2, size_y * 0.7);
    u32 mouth_y = offset + rand_u32_range(rng, eye_y, size_y);

    u32 eye_x = 0;

    f32 width = cfg_start_width;

    // Iterate 
    for (u32 y = offset; y < size.y-1; ++y) {
        if(width < 1) width = 1;

        for (u32 x = 0; x < size.x; ++x) {
            f32 cx = (f32)x - (f32)size.x / 2.0f + 0.5f;
            if (cx < 0) cx = -cx;

            if (cx < width) {
                image_write(image, (v2i){x, y}, color_base + rand_color(rng) * 0.05);
            }
        }

        if (y == eye_y) eye_x = width / 2;
        width += rand_f32_signed(rng) * cfg_spike;
    }

    u32 look_dir = rand_u32(rng) % 4;

    if (0) {
        image_write(image, (v2i){size.x / 2 - 3, mouth_y + 0}, (v4){1, 1, 1, 1});
        image_write(image, (v2i){size.x / 2 - 2, mouth_y + 1}, (v4){1, 1, 1, 1});
        image_write(image, (v2i){size.x / 2 - 1, mouth_y + 1}, (v4){1, 1, 1, 1});
        image_write(image, (v2i){size.x / 2 + 0, mouth_y + 1}, (v4){1, 1, 1, 1});
        image_write(image, (v2i){size.x / 2 + 1, mouth_y + 0}, (v4){1, 1, 1, 1});
    }

    v4 black = {0, 0, 0, 1};
    v4 white = {1, 1, 1, 1};

    image_write(image, (v2i){size.x / 2 - 1 - eye_x, eye_y}, look_dir == 1 ? black : white);
    image_write(image, (v2i){size.x / 2 - 2 - eye_x, eye_y}, look_dir == 0 ? black : white);
    image_write(image, (v2i){size.x / 2 - 1 - eye_x, eye_y + 1}, look_dir == 2 ? black : white);
    image_write(image, (v2i){size.x / 2 - 2 - eye_x, eye_y + 1}, look_dir == 3 ? black : white);

    image_write(image, (v2i){size.x / 2 + 0 + eye_x, eye_y}, look_dir == 0 ? black : white);
    image_write(image, (v2i){size.x / 2 + 1 + eye_x, eye_y}, look_dir == 1 ? black : white);
    image_write(image, (v2i){size.x / 2 + 0 + eye_x, eye_y + 1}, look_dir == 3 ? black : white);
    image_write(image, (v2i){size.x / 2 + 1 + eye_x, eye_y + 1}, look_dir == 2 ? black : white);

    if (rand_u32(rng) % 2 == 0) {
        image_write(image, (v2i){size.x / 2 + 2, offset - 0}, color_base);
        image_write(image, (v2i){size.x / 2 + 3, offset - 1}, color_base);
        image_write(image, (v2i){size.x / 2 + 4, offset - 2}, color_base);
        image_write(image, (v2i){size.x / 2 + 5, offset - 3}, color_base);

        image_write(image, (v2i){size.x / 2 + 6, offset - 3}, color_base);
        image_write(image, (v2i){size.x / 2 + 5, offset - 4}, color_base);
        image_write(image, (v2i){size.x / 2 + 6, offset - 4}, color_base);

        image_write(image, (v2i){size.x / 2 - 1 - 2, offset - 0}, color_base);
        image_write(image, (v2i){size.x / 2 - 1 - 3, offset - 1}, color_base);
        image_write(image, (v2i){size.x / 2 - 1 - 4, offset - 2}, color_base);
        image_write(image, (v2i){size.x / 2 - 1 - 5, offset - 3}, color_base);

        image_write(image, (v2i){size.x / 2 - 1 - 6, offset - 3}, color_base);
        image_write(image, (v2i){size.x / 2 - 1 - 5, offset - 4}, color_base);
        image_write(image, (v2i){size.x / 2 - 1 - 6, offset - 4}, color_base);
    }

    mon->color_base = color_base;
    mon->image = image;
    mon->eye_x = eye_x;
    mon->eye_y = eye_y;
    mon->mouth_y = mouth_y;
}

static Monster *monster_new(Memory *mem, Random *rng, v3 pos) {
    Monster *mon = mem_struct(mem, Monster);
    mon->pos = pos;
    mon->health = 10;
    monster_gen_image(mon, mem, rng);
    return mon;
}

static void monster_update(Monster *mon) {
}
