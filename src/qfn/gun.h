// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gun.h - Generate gun sprite
#pragma once
#include "gfx/color_rand.h"
#include "gfx/image.h"
#include "lib/mem.h"
#include "lib/rand.h"

static Image *gun_new(Memory *mem, Rand *rng) {
    u32 length = 8;
    u32 height = 3;

    Image *img = image_new(mem, (v2u){length, height + 3});
    image_fill(img, 0);

    v3 color_barrel = color_rand(rng) * 0.2;
    v3 color_sight = color_rand(rng) * 0.2;
    v3 color_grip = color_rand(rng) * 0.2;

    // Barrel
    for (u32 x = 0; x < length; ++x) {
        for (u32 y = 0; y < height; ++y) {
            image_write(img, (v2i){x, y + 1}, color_barrel);
        }
    }

    // Sight
    image_write(img, (v2i){1, 0}, color_sight);
    image_write(img, (v2i){length - 1, 0}, color_sight);

    for (u32 x = 0; x < 2; ++x) {
        image_write(img, (v2i){length - 1 - x, height + 1}, color_grip);
        image_write(img, (v2i){length - 1 - x, height + 2}, color_grip);
    }

    img->origin.x = length - 2;
    img->origin.y = img->size.y - 2;
    return img;
}
