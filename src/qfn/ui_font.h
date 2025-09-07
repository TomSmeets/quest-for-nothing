// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// ui.h - Immediate mode UI (Font)
#pragma once
#include "gfx/image.h"

// Render single character
static Image *ui_font_render(Memory *mem, u8 chr, v3 color) {
    // Remap chars
    if (chr >= 'a' && chr <= 'z') chr = chr - 'a' + 'A';

    // Create new
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

    v4 color_fg = {0, 0, 0, 1};
    v4 color_bg = {1, 1, 1, 1};

    Image *img = image_new(mem, (v2u){6, 6});
    // image_fill(img, color_bg);
    for (u32 y = 0; y < 5; ++y) {
        for (u32 x = 0; x < 5; ++x) {
            bool value = grid[(y * 5 + x) * 2] != ' ';
            if (!value) continue;
            img->pixels[y * 6 + x] = color_alpha(color, 1);
        }
    }
    return img;
}
