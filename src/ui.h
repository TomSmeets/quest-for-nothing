#pragma once

#include "audio.h"
#include "gfx.h"
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
    Image *chars[1 + 26 + 10];
} UI;

static void ui_init(UI *ui, Memory *mem, Audio *audio, Input *input, Gfx *gfx) {
    ui->audio = audio;
    ui->input = input;
    ui->gfx = gfx;
    ui->mem = mem;

    ui->image = image_new(mem, (v2u){16, 16});
    image_grid(ui->image, (v4){1, 0, 1, 1}, (v4){0, 0, 0, 1});
}

static void ui_begin(UI *ui) {
}

// Render single character
static Image *ui_get_char(UI *ui, u8 chr) {
    // Remap chars
    if (chr >= 'a' && chr <= 'z') chr = chr - 'a' + 'A';

    // Map char to index
    u32 index = 0;
    if (chr >= 'A' && chr <= 'Z') index = chr - 'A' + 1;
    if (chr >= '0' && chr <= '9') index = chr - '0' + 1 + 26;

    // Check Cache
    if (index == 0) return 0;
    if (ui->chars[index]) return ui->chars[index];

    // Create new
    Memory *mem = ui->mem;
    const char *grid;
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

    assert(str_len((char *)grid) == 5 * 5 * 2, "Character does not match size");

    v4 color_bg = {0, 0, 0, 0};
    v4 color_fg = {1, 1, 1, 1};

    Image *img = image_new(mem, (v2u){5, 5});
    for (u32 i = 0; i < 5 * 5; ++i) {
        bool value = grid[i * 2] != ' ';
        img->pixels[i] = value ? color_fg : color_bg;
    }
    ui->chars[index] = img;
    return img;
}
