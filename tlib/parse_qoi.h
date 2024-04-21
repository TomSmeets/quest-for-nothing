#pragma once
#include "tlib/image.h"
#include "tlib/mem.h"

struct qoi_hdr {
    u8 magic[4];
    u32 width;
    u32 height;
    u8 channels;
    u8 colorspace;
};

struct pixel {
    u8 r, g, b, a;
};

static void endian_swap(u32 *v) {
    u32 i = *v;
    u32 o = 0;

    o = (o << 8) | (i & 0xff);
    i >>= 8;
    o = (o << 8) | (i & 0xff);
    i >>= 8;
    o = (o << 8) | (i & 0xff);
    i >>= 8;
    o = (o << 8) | (i & 0xff);
    i >>= 8;
    *v = o;
}

static v4 px_to_col(pixel px) {
    v4 col = {px.r, px.g, px.b, px.a};
    col *= 1.0 / 255;

    // Convert to linear color space
    col.r = f_sqrt(col.r);
    col.g = f_sqrt(col.g);
    col.b = f_sqrt(col.b);
    col.a = col.a;
    return col;
}

static u32 pixel_hash(pixel px) {
    return (px.r * 3 + px.g * 5 + px.b * 7 + px.a * 11) % 64;
}

// Read a image file with the "Quite OK Image" format
static Image *parse_qoi(mem *m, Buffer file) {
    u8 *file_start = file.ptr;
    u8 *file_end = file.ptr + file.size;

    // 'p' is the current read cursor
    u8 *p = file_start;

    // Read the image header.
    // all integers are big endian,
    // so convert them to little endian
    qoi_hdr hdr = *(qoi_hdr *)p;
    endian_swap(&hdr.width);
    endian_swap(&hdr.height);

    // advance to the first bytes after the header
    p += 14;

    // for now I only support sRGB colorspace
    assert(hdr.colorspace == 0);

    // allocate memory for the image
    Image *img = img_new_uninit(m, hdr.width, hdr.height);

    // keep track of the last pixel, for run-length encoding
    pixel px = {.a = 255};

    // the pixel hash table, used for decompression
    pixel hash[64] = {};

    v4 *image_out = img->data;
    v4 *image_end = img->data + img->size_x * img->size_y;
    for (;;) {
        u8 op = *p++;
        if (image_out >= image_end)
            break;

        // OP_RGB
        if (op == 0xfe) {
            px.r = *p++;
            px.g = *p++;
            px.b = *p++;
            *image_out++ = px_to_col(px);
            // insert pixel into the hashtable
            hash[(px.r * 3 + px.g * 5 + px.b * 7 + px.a * 11) % 64] = px;
            continue;
        }

        // OP_RGBA
        if (op == 0xff) {
            px.r = *p++;
            px.g = *p++;
            px.b = *p++;
            px.a = *p++;
            *image_out++ = px_to_col(px);
            hash[(px.r * 3 + px.g * 5 + px.b * 7 + px.a * 11) % 64] = px;
            continue;
        }

        // OP_INDEX
        if ((op >> 6) == 0b00) {
            px = hash[op];
            *image_out++ = px_to_col(px);
            continue;
        }

        // OP_RUN
        if ((op >> 6) == 0b11) {
            u32 run = (op & 0b00111111) + 1;

            for (u32 i = 0; i < run; ++i) {
                *image_out++ = px_to_col(px);
            }
            continue;
        }

        // op diff
        if ((op >> 6) == 0b01) {
            u8 dr = (op >> 4) & 0b11;
            u8 dg = (op >> 2) & 0b11;
            u8 db = (op >> 0) & 0b11;
            px.r = px.r + dr - 2;
            px.g = px.g + dg - 2;
            px.b = px.b + db - 2;
            *image_out++ = px_to_col(px);
            hash[(px.r * 3 + px.g * 5 + px.b * 7 + px.a * 11) % 64] = px;
            continue;
        }

        // luma
        if ((op >> 6) == 0b10) {
            u8 arg = *p++;
            u8 dg = (op & 0b00111111) - 32;
            u8 dr = (arg >> 4) + dg - 8;
            u8 db = (arg & 0x0f) + dg - 8;
            px.r += dr;
            px.g += dg;
            px.b += db;
            *image_out++ = px_to_col(px);
            hash[(px.r * 3 + px.g * 5 + px.b * 7 + px.a * 11) % 64] = px;
            continue;
        }
    }

    return img;
}
