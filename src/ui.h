// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// ui.h - Immediate mode UI
#pragma once

#include "audio.h"
#include "gfx.h"
#include "str.h"
#include "vec.h"

typedef struct {
    v2 pos;
    v2 size;

    Audio *audio;
    Input *input;
    Gfx *gfx;

    Memory *mem;

    Image *image;

    // NULL, A-Z, 0-9
    Image *chars[256];
} UI;

static UI *ui_new(Memory *mem, Audio *audio, Input *input, Gfx *gfx) {
    UI *ui = mem_struct(mem, UI);
    ui->audio = audio;
    ui->input = input;
    ui->gfx = gfx;
    ui->mem = mem;

    ui->image = image_new(mem, (v2u){16, 16});
    image_grid(ui->image, (v4){1, 0, 1, 1}, (v4){0, 0, 0, 1});
    return ui;
}

static void ui_begin(UI *ui) {
}

// Render single character
static Image *ui_get_char(UI *ui, u8 chr) {
    // Remap chars
    if (chr >= 'a' && chr <= 'z') chr = chr - 'a' + 'A';

    // Map char to index

    // Check Cache
    if (ui->chars[chr]) return ui->chars[chr];

    // Create new
    Memory *mem = ui->mem;
    const char *grid = 0;
    if (chr == 'A')
        grid = "  x x x   "
               "x       x "
               "x x x x x "
               "x       x "
               "x       x ";

    if (chr == 'B')
        grid = "x x x x   "
               "x       x "
               "x x x x   "
               "x       x "
               "x x x x   ";

    if (chr == 'C')
        grid = "  x x x x "
               "x         "
               "x         "
               "x         "
               "  x x x x ";

    if (chr == 'D')
        grid = "x x x x   "
               "x       x "
               "x       x "
               "x       x "
               "x x x x   ";

    if (chr == 'E')
        grid = "x x x x x "
               "x         "
               "x x x x x "
               "x         "
               "x x x x x ";

    if (chr == 'F')
        grid = "x x x x x "
               "x         "
               "x x x x x "
               "x         "
               "x         ";

    if (chr == 'G')
        grid = "x x x x x "
               "x         "
               "x   x x x "
               "x       x "
               "x x x x x ";

    if (chr == 'H')
        grid = "x       x "
               "x       x "
               "x x x x x "
               "x       x "
               "x       x ";

    if (chr == 'I')
        grid = "x x x x x "
               "    x     "
               "    x     "
               "    x     "
               "x x x x x ";

    if (chr == 'J')
        grid = "  x x x x "
               "        x "
               "        x "
               "x       x "
               "  x x x   ";

    if (chr == 'K')
        grid = "x       x "
               "x     x   "
               "x x x     "
               "x     x   "
               "x       x ";

    if (chr == 'L')
        grid = "x         "
               "x         "
               "x         "
               "x         "
               "x x x x x ";

    if (chr == 'M')
        grid = "x       x "
               "x x   x x "
               "x   x   x "
               "x       x "
               "x       x ";

    if (chr == 'N')
        grid = "x       x "
               "x x     x "
               "x   x   x "
               "x     x x "
               "x       x ";

    if (chr == 'O')
        grid = "  x x x   "
               "x       x "
               "x       x "
               "x       x "
               "  x x x   ";

    if (chr == 'P')
        grid = "x x x x   "
               "x       x "
               "x x x x   "
               "x         "
               "x         ";

    if (chr == 'Q')
        grid = "  x x x   "
               "x       x "
               "x       x "
               "x     x x "
               "  x x x x ";

    if (chr == 'R')
        grid = "x x x x   "
               "x       x "
               "x x x x   "
               "x     x   "
               "x       x ";

    if (chr == 'S')
        grid = "  x x x x "
               "x         "
               "  x x x   "
               "        x "
               "x x x x   ";

    if (chr == 'T')
        grid = "x x x x x "
               "    x     "
               "    x     "
               "    x     "
               "    x     ";

    if (chr == 'U')
        grid = "x       x "
               "x       x "
               "x       x "
               "x       x "
               "  x x x   ";

    if (chr == 'V')
        grid = "x       x "
               "x       x "
               "x       x "
               "  x   x   "
               "    x     ";

    if (chr == 'W')
        grid = "x       x "
               "x   x   x "
               "x   x   x "
               "x   x   x "
               "  x   x   ";

    if (chr == 'X')
        grid = "x       x "
               "  x   x   "
               "    x     "
               "  x   x   "
               "x       x ";

    if (chr == 'Y')
        grid = "x       x "
               "  x   x   "
               "    x     "
               "    x     "
               "    x     ";

    if (chr == 'Z')
        grid = "x x x x x "
               "      x   "
               "    x     "
               "  x       "
               "x x x x x ";

    if (chr == '!')
        grid = "    x     "
               "    x     "
               "    x     "
               "          "
               "    x     ";

    if (chr == '0')
        grid = "  x x x   "
               "x       x "
               "x       x "
               "x       x "
               "  x x x   ";

    if (chr == '1')
        grid = "  x x     "
               "x   x     "
               "    x     "
               "    x     "
               "x x x x x ";

    if (chr == '2')
        grid = "x x x x   "
               "        x "
               "  x x x   "
               "x         "
               "x x x x x ";

    if (chr == '3')
        grid = "x x x x   "
               "        x "
               "  x x x   "
               "        x "
               "x x x x   ";

    if (chr == '4')
        grid = "    x x   "
               "  x   x   "
               "x     x   "
               "x x x x x "
               "      x   ";

    if (chr == '5')
        grid = "x x x x x "
               "x         "
               "x x x x   "
               "        x "
               "x x x x   ";

    if (chr == '6')
        grid = "  x x x x "
               "x         "
               "x x x x   "
               "x       x "
               "  x x x   ";

    if (chr == '7')
        grid = "x x x x x "
               "        x "
               "      x   "
               "    x     "
               "  x       ";

    if (chr == '8')
        grid = "  x x x   "
               "x       x "
               "  x x x   "
               "x       x "
               "  x x x   ";

    if (chr == '9')
        grid = "  x x x   "
               "x       x "
               "  x x x x "
               "        x "
               "  x x x   ";

    if (chr == '.')
        grid = "          "
               "          "
               "          "
               "          "
               "    x     ";

    if (chr == ',')
        grid = "          "
               "          "
               "          "
               "    x     "
               "  x       ";

    if (chr == ' ')
        grid = "          "
               "          "
               "          "
               "          "
               "          ";

    if (!grid)
        grid = "x   x   x "
               "          "
               "x       x "
               "          "
               "x   x   x ";

    assert(str_len((char *)grid) == 5 * 5 * 2, "Character does not match size");

    v4 color_bg = {0, 0, 0, 1};
    v4 color_fg = {1, 1, 1, 1};

    Image *img = image_new(mem, (v2u){6, 6});
    image_fill(img, color_bg);
    for (u32 y = 0; y < 5; ++y) {
        for (u32 x = 0; x < 5; ++x) {
            bool value = grid[(y * 5 + x) * 2] != ' ';
            if (value) img->pixels[y * 6 + x] = color_fg;
        }
    }
    ui->chars[chr] = img;
    return img;
}

static void ui_text(UI *ui, m4 mtx, char *text) {
    f32 x = 0;
    f32 y = 0;
    for (;;) {
        char c = *text++;
        if (!c) break;

        if (c == '\n') {
            x = 0;
            y -= 6 * 4;
            continue;
        }

        Image *img = ui_get_char(ui, c);
        if (img) {
            m4 mtx2 = m4_id();
            m4_translate_x(&mtx2, x);
            m4_translate_y(&mtx2, y);
            m4_apply(&mtx2, mtx);
            gfx_quad_ui(ui->gfx, mtx2, img);
        }

        x += 6 * 4;
    }
}
