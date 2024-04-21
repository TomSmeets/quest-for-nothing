// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// image.h - Handle graphical images in f32 rgba format
#pragma once
#include "tlib/color.h"
#include "tlib/inc.h"
#include "tlib/mem.h"

// A f32 rgba image in linear colorspace
struct Image {
    u32 size_x;
    u32 size_y;
    v4 *data; // rgba
};

// Create a new image with a given size
// Note: the image data is uninitialised, so clear it first
static Image *img_new_uninit(mem *m, u32 sx, u32 sy) {
    Image *img = mem_struct(m, Image);
    img->size_x = sx;
    img->size_y = sy;
    img->data = mem_array_uninit(m, v4, sx * sy);
    return img;
}

// Fill entire image with a single color
static void img_fill(Image *img, v4 value) {
    for (u32 i = 0; i < img->size_y * img->size_x; ++i) {
        img->data[i] = value;
    }
}

static void img_fill_pattern(Image *img) {
    for (u32 i = 0; i < img->size_y * img->size_x; ++i) {
        v4 value = BLACK;

        u32 x = i % img->size_x;
        u32 y = i / img->size_x;
        if (((x ^ y) & 1) == 0)
            value = WHITE;
        img->data[i] = value;
    }
}

// Extract a sub image from a given region in the source image
static Image *img_copy_region(mem *m, Image *src, u32 px, u32 py, u32 sx, u32 sy) {
    assert(px + sx <= src->size_x);
    assert(py + sy <= src->size_y);

    Image *img = img_new_uninit(m, sx, sy);
    for (u32 dy = 0; dy < sy; ++dy) {
        for (u32 dx = 0; dx < sx; ++dx) {
            img->data[dy * sx + dx] = src->data[(py + dy) * src->size_x + px + dx];
        }
    }
    return img;
}

// Create a exact copy of the source image
static Image *img_clone(mem *m, Image *src) {
    Image *img = img_new_uninit(m, src->size_x, src->size_y);
    std_memcpy(img->data, src->data, src->size_x * src->size_y * sizeof(v4));
    return img;
}

// Multiply this image by a given color
static void img_tint(Image *img, v4 col) {
    for (u32 i = 0; i < img->size_x * img->size_y; ++i)
        img->data[i] *= col;
}

static v4 img_pixel_at(Image *img, v2i pos) {
    if (pos.x >= (i32)img->size_x)
        return 0;
    if (pos.y >= (i32)img->size_y)
        return 0;
    if (pos.x < 0)
        return 0;
    if (pos.y < 0)
        return 0;
    return img->data[(u32)(pos.y * img->size_x + pos.x)];
}

static v4 img_pixel_at_uv(Image *img, v2 uv) {
    v2i p = {};
    p.x = uv.x * img->size_x;
    p.y = (1 - uv.y) * img->size_y;
    return img_pixel_at(img, p);
}
