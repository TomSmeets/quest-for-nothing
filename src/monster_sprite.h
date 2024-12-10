// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster_sprite.h - Generate Sprites for Aliens
#pragma once
#include "color.h"
#include "gfx.h"
#include "image.h"
#include "player.h"
#include "rand.h"
#include "vec.h"

typedef struct {
    // Left and right eye positions
    v2i eye[2];

    // Generated image
    Image *image;

    u32 eye_count;
} Monster_Sprite;

// Update monster eye position
static void monster_sprite_update_eyes(Monster_Sprite *mon, Random *rng) {
    v3 black = {0, 0, 0};
    v3 white = {1, 1, 1};

    u32 look_dir = rand_u32(rng) % 4;
    for (u32 i = 0; i < mon->eye_count; ++i) {
        v2i eye = mon->eye[i];
        image_write(mon->image, eye + (v2i){0, 0}, look_dir == 0 ? black : white);
        image_write(mon->image, eye + (v2i){1, 0}, look_dir == 1 ? black : white);
        image_write(mon->image, eye + (v2i){0, 1}, look_dir == 3 ? black : white);
        image_write(mon->image, eye + (v2i){1, 1}, look_dir == 2 ? black : white);
    }
    mon->image->id = id_next();
}

static Monster_Sprite monster_sprite_generate(Memory *mem, Random *rng) {
    float texture = rand_f32_range(rng, 0.02, 0.05);

    // Initial line widths
    u32 body_height = rand_u32_range(rng, 12, 32);
    u32 start_radius = rand_u32_range(rng, 1, 8);
    u32 eye_y = rand_u32_range(rng, 0, body_height / 2.0f);
    float spike = rand_f32_range(rng, 0.5, 3.0);
    v3 color_base = rand_color(rng);

    u32 eye_x = 0;       // Computed later
    u32 body_radius = 0; // Computed later
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
    image_fill(image, color_alpha(color_base, 0));
    // image_grid(image, WHITE, GRAY);
    // image_write_debug_axis(image);

    for (u32 y = 0; y < body_height; ++y) {
        f32 r = radius_list[y];

        f32 dist_y = (f32)y / (f32)body_height;
        dist_y = (dist_y * 2 - 1) * (dist_y * 2 - 1);
        dist_y = dist_y * 0.8;

        for (u32 x = 0; x < size.x; ++x) {
            f32 d = (f32)x - (f32)(size.x - 1) / 2.0f;
            if (d > -r && d < r) {
                f32 dist = (d * d) / (r * r);
                dist = f_max(dist, dist_y);

                // Start with the base color
                v3 color = color_base;

                // Add a textured surface
                color += rand_v3(rng) * texture * (1 - dist);
                color *= 1.0 - dist * 0.2;

                image_write(image, (v2i){x, y}, color);
            }
        }
    }
    Monster_Sprite mon = {
        .image = image,
    };

    if (eye_x <= 1) eye_x = 0;

    mon.eye[mon.eye_count++] = (v2i){image->size.x / 2 + eye_x - 1, eye_y};

    // This is not centered, but I like this even more tbh
    if (eye_x > 0) mon.eye[mon.eye_count++] = (v2i){image->size.x / 2 - 1 - eye_x, eye_y};
    monster_sprite_update_eyes(&mon, rng);
    image_write_debug_axis(image);
    return mon;
}
