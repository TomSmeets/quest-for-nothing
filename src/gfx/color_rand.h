#pragma once
#include "gfx/color.h"
#include "lib/rand.h"

static v3 color_rand(Rand *rand) {
    return (v3){
        rand_f32(rand, 0, 1),
        rand_f32(rand, 0, 1),
        rand_f32(rand, 0, 1),
    };
}

static v3 color_rand_rainbow(Rand *rand) {
    f32 v = rand_f32(rand, 0, 1);
    f32 r = 0.5f + 0.5 * f_cos2pi(v + 0.0f / 3.0f);
    f32 g = 0.5f + 0.5 * f_cos2pi(v + 1.0f / 3.0f);
    f32 b = 0.5f + 0.5 * f_cos2pi(v + 2.0f / 3.0f);
    return (v3){r, g, b};
}
