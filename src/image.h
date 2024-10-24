// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// image.h - Image type
#pragma once
#include "color.h"
#include "id.h"
#include "mem.h"
#include "vec.h"

typedef struct {
    // Unique id, update on every change
    u32 id;
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
    for (u32 i = 0; i < img->size.x * img->size.y; ++i) {
        img->pixels[i] = color;
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

static void image_write(Image *img, v2i pos, v4 value) {
    if (pos.x < 0 || pos.x >= img->size.x) return;
    if (pos.y < 0 || pos.y >= img->size.y) return;
    img->pixels[pos.y * img->size.x + pos.x] = value;
}

static void image_write_debug_axis(Image *img) {
    image_write(img, (v2i){0, 0}, BLACK);
    for (u32 x = 1; x < img->size.x; ++x) {
        image_write(img, (v2i){x, 0}, RED);
        image_write(img, (v2i){x, img->size.y - 1}, WHITE);
    }
    for (u32 y = 1; y < img->size.y; ++y) {
        image_write(img, (v2i){0, y}, GREEN);
        image_write(img, (v2i){img->size.x - 1, y}, WHITE);
    }
}
