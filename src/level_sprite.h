// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// level_sprite.h - Generate artwork for spaceship walls
#pragma once

#include "image.h"
#include "rand.h"

static f32 color_splatter(Random *rng, f32 chance, f32 min, f32 max) {
    if (rand_f32(rng) < chance) return rand_f32_range(rng, min, max);
    return 0;
}

static Image *level_sprite_generate(Memory *mem, Random *rng) {
    float dirt = 0;
    float moss = 0;

    u32 sx = 32 * 4;
    u32 sy = 32 * 4;

    Image *img = image_new(mem, (v2u){sx, sy});
    v3 tint = rand_color(rng);
    v3 tint2 = rand_color(rng);
    for (u32 y = 0; y < sy; ++y) {
        for (u32 x = 0; x < sx; ++x) {
            f32 r_x = f_min(x, sx - x - 1);
            f32 r_y = f_min(y, sy - y - 1);
            f32 r = f_min(r_x, r_y);

            f32 dirt = f_max(rand_f32(rng) * 2 - 1, 0);

            v3 color = color_blend(WHITE, tint, 0.1);
            color = color_blend(color, tint2, color_splatter(rng, 0.8, 0.0, 0.1));
            // color = color_blend(color, (v3){27, 94, 32} / 255.0f, color_splatter(rng, 0.01, 0.2, 0.8));

            // Border
            color = color_blend(color, (v3){.3, .3, .3}, f_clamp(1 - r * 0.3, 0, 1));

            image_write(img, (v2i){x, y}, color);
        }
    }
    // image_write_debug_axis(img);
    return img;
}
