// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Graphics, Audio, and Input handling abstraction implementation
#pragma once
#include "gfx_api.h"
#include "os_api.h"

#if OS_IS_LINUX || OS_IS_WINDOWS
#include "gfx_desktop.h"
#elif OS_IS_WASM
#include "gfx_wasm.h"
#endif

// Render a flat upright sprite facing the camera
static void os_gfx_sprite(Gfx *gfx, v3 player, v3 pos, Image *img, float wiggle) {
    // Relative direction to the camera in xz
    v3 dir = pos - player;
    dir.y = 0;

    v3 z = v3_normalize(dir);
    v3 x = {z.z, 0, -z.x};
    v3 y = {0, 1, 0};

    y += x * wiggle;

    v2 size = v2u_to_v2(img->size) / 32.0;

    m4s mtx = m4s_id();
    mtx.x.xyz = x * size.x;
    mtx.y.xyz = y * size.y;
    mtx.z.xyz = z;
    mtx.w.xyz = pos + (v3){0, size.y * .5f, 0} + x * wiggle * .5;
    os_gfx_quad(gfx, &mtx, img);
}
