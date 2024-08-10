// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// image.h - Image type
#pragma once
#include "vec.h"
#include "mem.h"

typedef struct {
    v2u size;
    u32 *pixels;
} Image;

static Image *img_new(Memory *mem, v2u size) {
    Image *img = mem_struct(mem, Image);
    img->size = size;
    img->pixels = mem_array_uninit(mem, u32, size.x*size.y);
    return img;
}

static void img_fill(Image *img, u32 color) {
    for(u32 y = 0; y < img->size.y; ++y) {
        for(u32 x = 0; x < img->size.x; ++x) {
            img->pixels[y*img->size.x + x] = color;
        }
    }
}

static u32 color_rgba(u8 r, u8 g, u8 b, u8 a) {
    u32 color = 0;
    color = (color << 8) | r;
    color = (color << 8) | g;
    color = (color << 8) | b;
    color = (color << 8) | a;
    return color;
}

static u32 color_rgb(u8 r, u8 g, u8 b) {
    return color_rgba(r, g, b, 0xff);
}
