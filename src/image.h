// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// image.h - Image type
#pragma once
#include "id.h"
#include "mem.h"
#include "vec.h"

typedef struct {
    u64 id;
    v2u size;
    v4 *pixels;
} Image;

static Image *image_new(Memory *mem, v2u size) {
    Image *img = mem_struct(mem, Image);
    img->id = id_next();
    img->size = size;
    img->pixels = mem_array_uninit(mem, v4, size.x * size.y);
    return img;
}

static void image_fill(Image *img, v4 color) {
    for (u32 y = 0; y < img->size.y; ++y) {
        for (u32 x = 0; x < img->size.x; ++x) {
            img->pixels[y * img->size.x + x] = color;
        }
    }
    img->id = id_next();
}

static Image *image_grid(Image *img, v4 c1, v4 c2) {
    for (u32 y = 0; y < img->size.y; y++) {
        for (u32 x = 0; x < img->size.x; x++) {
            img->pixels[y * img->size.y + x] = (x % 2 == y % 2) ? c1 : c2;
        }
    }
    return img;
}
