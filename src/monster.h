// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "color.h"
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
    float texture = 0.05;

    // Initial line widths
    u32 body_height = rand_u32_range(rng, 12, 32);
    u32 start_radius = rand_u32_range(rng, 1, 8);
    u32 eye_y = rand_u32_range(rng, 0, body_height / 2.0f);
    u32 eye_x = 0;       // Computed later
    u32 body_radius = 0; // Computed later

    // Parameters
    float spike = rand_f32_range(rng, 0.5, 3.0);
    v4 color_base = rand_color(rng);

    f32 *radius_list = mem_array_uninit(mem, f32, body_height);

    f32 radius = start_radius;
    for (u32 i = 0; i < body_height; ++i) {
        radius_list[i] = radius;
        if (radius > body_radius) body_radius = radius;
        if (i == eye_y) eye_x = radius / 2.0f;

        radius += rand_f32_signed(rng) * spike;
        if (radius < 1) radius = 1;
    }

    // Image size
    v2u size = {body_radius * 2 + 2, body_height};

    // Clear image with base color
    // This improves alpha blending
    // Probably go to premultiplied alpha
    Image *image = image_new(mem, size);
    image_fill(image, color_base * (v4){1, 1, 1, 0});
    // image_write_debug_axis(image);

    for (u32 y = 0; y < body_height; ++y) {
        f32 r = radius_list[y];
        for (u32 x = 0; x < size.x; ++x) {
            f32 d = (f32)x - (f32)(size.x - 1) / 2.0f;
            if (d > -r && d < r) {
                image_write(image, (v2i){x, y}, color_base + rand_color(rng) * texture);
            }
        }
    }

    u32 look_dir = rand_u32(rng) % 4;
    v4 black = {0, 0, 0, 1};
    v4 white = {1, 1, 1, 1};

    image_write(image, (v2i){size.x / 2 + 0 + eye_x, eye_y}, look_dir == 1 ? black : white);
    image_write(image, (v2i){size.x / 2 + 1 + eye_x, eye_y}, look_dir == 0 ? black : white);
    image_write(image, (v2i){size.x / 2 + 0 + eye_x, eye_y + 1}, look_dir == 2 ? black : white);
    image_write(image, (v2i){size.x / 2 + 1 + eye_x, eye_y + 1}, look_dir == 3 ? black : white);

    image_write(image, (v2i){size.x / 2 - 1 - eye_x, eye_y}, look_dir == 0 ? black : white);
    image_write(image, (v2i){size.x / 2 - 2 - eye_x, eye_y}, look_dir == 1 ? black : white);
    image_write(image, (v2i){size.x / 2 - 1 - eye_x, eye_y + 1}, look_dir == 3 ? black : white);
    image_write(image, (v2i){size.x / 2 - 2 - eye_x, eye_y + 1}, look_dir == 2 ? black : white);

    mon->color_base = color_base;
    mon->image = image;
    mon->eye_x = eye_x;
    mon->eye_y = eye_y;
    // mon->mouth_y = mouth_y;
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
