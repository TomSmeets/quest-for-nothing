// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "color.h"
#include "image.h"
#include "player.h"
#include "rand.h"
#include "vec.h"

typedef struct Monster {
    v3 pos;
    v3 old_pos;

    v2 move_dir;
    f32 move_time;

    u32 health;
    u32 eye_x;
    u32 eye_y;
    Image *image;
    struct Monster *next;
} Monster;

static void monster_set_eyes(Monster *mon, Random *rng) {
    u32 look_dir = rand_u32(rng) % 4;
    v4 black = {0, 0, 0, 1};
    v4 white = {1, 1, 1, 1};

    image_write(mon->image, (v2i){mon->image->size.x / 2 + 0 + mon->eye_x, mon->eye_y}, look_dir == 1 ? black : white);
    image_write(mon->image, (v2i){mon->image->size.x / 2 + 1 + mon->eye_x, mon->eye_y}, look_dir == 0 ? black : white);
    image_write(mon->image, (v2i){mon->image->size.x / 2 + 0 + mon->eye_x, mon->eye_y + 1}, look_dir == 2 ? black : white);
    image_write(mon->image, (v2i){mon->image->size.x / 2 + 1 + mon->eye_x, mon->eye_y + 1}, look_dir == 3 ? black : white);

    image_write(mon->image, (v2i){mon->image->size.x / 2 - 1 - mon->eye_x, mon->eye_y}, look_dir == 0 ? black : white);
    image_write(mon->image, (v2i){mon->image->size.x / 2 - 2 - mon->eye_x, mon->eye_y}, look_dir == 1 ? black : white);
    image_write(mon->image, (v2i){mon->image->size.x / 2 - 1 - mon->eye_x, mon->eye_y + 1}, look_dir == 3 ? black : white);
    image_write(mon->image, (v2i){mon->image->size.x / 2 - 2 - mon->eye_x, mon->eye_y + 1}, look_dir == 2 ? black : white);
    mon->image->id = id_next();
}

static void monster_gen_image(Monster *mon, Memory *mem, Random *rng) {
    float texture = 0.05;

    // Initial line widths
    u32 body_height = rand_u32_range(rng, 12, 32);
    u32 start_radius = rand_u32_range(rng, 1, 8);
    u32 eye_y = rand_u32_range(rng, 0, body_height / 2.0f);
    u32 eye_x = 0;       // Computed later
    u32 body_radius = 0; // Computed later

    // Parameters
    float spike = rand_f32_range(rng, 0.5, 3.0);
    v4 color_base = rand_color(rng);

    f32 *radius_list = mem_array_uninit(mem, f32, body_height);

    f32 radius = start_radius;
    for (u32 i = 0; i < body_height; ++i) {
        radius_list[i] = radius;
        if (radius > body_radius) body_radius = radius;
        if (i == eye_y) eye_x = radius / 2.0f;

        radius += rand_f32_signed(rng) * spike;
        if (radius < 1) radius = 1;
    }

    // Image size
    v2u size = {body_radius * 2 + 2, body_height};

    // Clear image with base color
    // This improves alpha blending
    // Probably go to premultiplied alpha
    Image *image = image_new(mem, size);
    image_fill(image, color_base * (v4){1, 1, 1, 0});
    // image_write_debug_axis(image);

    for (u32 y = 0; y < body_height; ++y) {
        f32 r = radius_list[y];
        for (u32 x = 0; x < size.x; ++x) {
            f32 d = (f32)x - (f32)(size.x - 1) / 2.0f;
            if (d > -r && d < r) {
                image_write(image, (v2i){x, y}, color_base + rand_color(rng) * texture);
            }
        }
    }
    mon->image = image;
    mon->eye_x = eye_x;
    mon->eye_y = eye_y;
    monster_set_eyes(mon, rng);
}

static Monster *monster_new(Memory *mem, Random *rng, v3 pos) {
    Monster *mon = mem_struct(mem, Monster);
    mon->pos = pos;
    mon->old_pos = pos;
    mon->health = 10;
    monster_gen_image(mon, mem, rng);
    return mon;
}

static void monster_collide(Monster *m1, Monster *m2) {
    v3 diff = m1->pos - m2->pos;
    f32 len = v3_length(diff);
    f32 radius = 0.40f;
    if (len > radius) return;
    if (len <= 0) return;

    v3 push = diff / len * (radius - len);
    m1->pos += push * 0.25;
    m2->pos -= push * 0.25;
}

static void monster_update(Monster *mon, f32 dt, Player *p, Random *rng) {
    mon->old_pos = mon->pos;
    mon->pos.xz += mon->move_dir * dt;

    mon->move_time -= dt;
    if (mon->move_time <= 0) {
        mon->move_time = rand_f32_range(rng, 2, 10);

        u32 mode = rand_u32_range(rng, 0, 1);
        fmt_su(OS_FMT, "mode=", mode, "\n");
        if (mode == 0) mon->move_dir = 0;
        // if (mode == 1) mon->move_dir = v2_normalize(p->pos.xz - mon->pos.xz)*0.1;
        if (mode == 1) mon->move_dir = v2_from_rot(rand_f32_signed(rng) * PI) * 0.25;
        monster_set_eyes(mon, rng);
    }

    // v3 old = mon->old_pos;
    // mon->old_pos = mon->pos;
    // mon->pos += mon->pos - old;

    // v3 vel = mon->pos - mon->old_pos;
    // vel.xz = v2_limit(vel.xz, 0.01f * dt, 5.0f / 3.6f * dt);
    // vel.xz *= 1.0f - 0.01;
    // mon->old_pos = mon->pos - vel;
}
