// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// level_sprite.h - Generate artwork for spaceship walls
#pragma once
#include "color.h"
#include "color_rand.h"
#include "image.h"
#include "math.h"
#include "rand.h"

static void color_splatter(v3 *color, Rand *rng, v3 tint, f32 chance, f32 min, f32 max) {
    if (!rand_choice(rng, chance)) return;
    *color = color_blend(*color, tint, rand_f32(rng, min, max));
}

static Image *level_sprite_generate(Memory *mem, Rand *rng) {
    u32 sx = 32 * 4;
    u32 sy = 32 * 4;

    Image *img = image_new(mem, (v2u){sx, sy});
    v3 tint1 = color_rand(rng);
    v3 tint2 = color_rand(rng);
    for (u32 y = 0; y < sy; ++y) {
        for (u32 x = 0; x < sx; ++x) {
            f32 r_x = f_min(x, sx - x - 1);
            f32 r_y = f_min(y, sy - y - 1);
            f32 r = f_min(r_x, r_y);

            f32 dirt = f_max(rand_f32(rng, -1, 1), 0);

            v3 color = color_blend(WHITE, tint1, 0.4);
            color_splatter(&color, rng, tint2, 0.8, 0.0, 0.2);
            // color_splatter(&color, rng, (v3){27, 94, 32} / 255.0f, 0.01, 0.2, 0.8);

            // Border
            color = color_blend(color, (v3){.3, .3, .3}, f_clamp(1 - r * 0.3, 0, 1));

            image_write(img, (v2i){x, y}, color);
        }
    }
    // image_write_debug_axis(img);
    return img;
}
