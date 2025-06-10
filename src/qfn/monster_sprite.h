// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster_sprite.h - Generate Sprites for Aliens
#pragma once
#include "lib/math.h"
#include "lib/mem.h"
#include "lib/rand.h"
#include "lib/vec.h"
#include "qfn/color.h"
#include "qfn/color_rand.h"
#include "qfn/image.h"
#include "qfn/mat.h"

typedef struct {
    // Left and right eye positions
    u32 eye_count;
    v2i eye[2];
    v2i hand[2];

    v3 base_color;
    v3 blood_color;

    // Generated image
    Image *image;
    Image *shadow;
} Monster_Sprite;

// Update monster eye position
static void monster_sprite_update_eyes(Monster_Sprite *mon, Rand *rng) {
    v3 black = {0, 0, 0};
    v3 white = {1, 1, 1};

    u32 look_dir = rand_u32(rng, 0, 4);
    for (u32 i = 0; i < mon->eye_count; ++i) {
        v2i eye = mon->eye[i];
        image_write(mon->image, eye + (v2i){0, 0}, look_dir == 0 ? black : white);
        image_write(mon->image, eye + (v2i){1, 0}, look_dir == 1 ? black : white);
        image_write(mon->image, eye + (v2i){0, 1}, look_dir == 3 ? black : white);
        image_write(mon->image, eye + (v2i){1, 1}, look_dir == 2 ? black : white);
    }
    mon->image->variation++;
}

static Image *monster_gen_shadow(Memory *mem, u32 size) {
    Image *img = image_new(mem, (v2u){size, size});
    f32 r = (f32)size / 2.0f;
    for (u32 y = 0; y < size; ++y) {
        for (u32 x = 0; x < size; ++x) {
            v2 pos = {(f32)x + 0.5f, (f32)y + 0.5f};
            v2 center = {r, r};
            f32 dist = v2_length(pos - center);
            f32 color = 0.0;
            f32 alpha = dist < r ? 1.0 : 0.0;
            image_write4(img, (v2i){x, y}, (v4){color, color, color, alpha});
        }
    }
    return img;
}

// Properties for generating a monster
typedef struct {
    // How textured the skin is
    f32 texture;

    // Monster size
    f32 body_height;

    // How thick is the top part of the head
    f32 start_radius;

    f32 eye_y;
    f32 hand_y;

    f32 spike;

    v3 color_base;
    v3 color_accent;
    v3 color_blood;
} Sprite_Properties;

static Sprite_Properties sprite_new(Rand *rng) {
    return (Sprite_Properties){
        .texture = rand_f32(rng, 0.02, 0.08),
        .body_height = rand_f32(rng, 12, 32),
        .start_radius = rand_f32(rng, 1, 8),
        .eye_y = rand_f32(rng, 0, 0.5),
        .hand_y = rand_f32(rng, 0.5, 0.7),
        .spike = rand_f32(rng, 0.5, 3.0),
        .color_base = color_rand(rng),
        .color_accent = color_rand(rng),
        .color_blood = color_rand_rainbow(rng),
    };
}

// Blend between two sprites
static void sprite_blend(Sprite_Properties *x, Sprite_Properties y, f32 amount) {
#define blend(FIELD) x->FIELD += (y.FIELD - x->FIELD) * amount
    blend(texture);
    blend(body_height);
    blend(start_radius);
    blend(eye_y);
    blend(hand_y);
    blend(spike);
    blend(color_base);
    blend(color_accent);
    blend(color_blood);
#undef blend
}

static Monster_Sprite monster_sprite_generate(Memory *mem, Sprite_Properties prop, Rand *rng) {
    // Initial line widths
    u32 body_height = f_round(prop.body_height);
    u32 eye_y = prop.eye_y * prop.body_height;
    u32 hand_y = prop.hand_y * prop.body_height;
    u32 eye_x = 0;
    u32 hand_x = 0;
    u32 body_radius = 0;

    f32 *radius_list = mem_array_uninit(mem, f32, body_height);
    f32 radius = prop.start_radius;
    for (u32 i = 0; i < body_height; ++i) {
        radius_list[i] = radius;
        if (radius > body_radius) body_radius = radius;
        if (i == eye_y) eye_x = radius / 2.0f;
        if (i == hand_y) hand_x = radius / 2.0f;
        radius += rand_f32(rng, -prop.spike, prop.spike);
        if (radius < 1) radius = 1;
    }

    // Image size
    v2u size = {body_radius * 2 + 2, body_height};

    // Clear image with base color
    // This improves alpha blending
    // Probably go to premultiplied alpha
    Image *image = image_new(mem, size);
    image_fill(image, color_alpha(prop.color_base, 0));
    // image_grid(image, WHITE, GRAY);

    for (u32 y = 0; y < body_height; ++y) {
        f32 r = radius_list[y];

        f32 dist_y = (f32)y / (f32)body_height;
        dist_y = (dist_y * 2 - 1) * (dist_y * 2 - 1);
        dist_y = dist_y * 0.8;

        for (u32 x = 0; x < size.x; ++x) {
            f32 d = (f32)x - (f32)(size.x - 1) / 2.0f;
            if (d > -r && d < r) {
                f32 dist = (d * d) / (r * r);
                dist = f_max(dist, dist_y);

                // Start with the base color
                v3 color = color_blend(prop.color_base, prop.color_accent, (f32)y / (body_height - 1) * 0.5);

                // Add a textured surface
                color += color_rand(rng) * prop.texture * (1 - dist);
                color *= 1.0 - dist * 0.2;

                image_write(image, (v2i){x, y}, color);
            }
        }
    }

    if (eye_x <= 1) eye_x = 0;

    // Image
    Monster_Sprite sprite = {};
    sprite.base_color = prop.color_base;
    sprite.blood_color = prop.color_blood;

    sprite.image = image;
    sprite.eye[sprite.eye_count++] = (v2i){image->size.x / 2 + eye_x - 1, eye_y};
    sprite.hand[0] = (v2i){image->size.x / 2 + hand_x, hand_y};
    sprite.hand[1] = (v2i){image->size.x / 2 - 1 - hand_x, hand_y};
    // This is not centered, but I like this even more tbh
    if (eye_x > 0) sprite.eye[sprite.eye_count++] = (v2i){image->size.x / 2 - 1 - eye_x, eye_y};

    image->origin.x = image->size.x / 2;
    image->origin.y = image->size.y;

    // Update other eyes
    monster_sprite_update_eyes(&sprite, rng);
    sprite.shadow = monster_gen_shadow(mem, image->size.x * 0.80f);
    return sprite;
}

static void m4_scale_image(m4 *mtx, Image *img) {
    m4_scale(mtx, (v3){img->size.x / 32.0f, img->size.y / 32.0f, 1});
}
