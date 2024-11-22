// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "color.h"
#include "gfx.h"
#include "image.h"
#include "player.h"
#include "rand.h"
#include "vec.h"

typedef struct Monster {
    // ==== Physics ====
    v3 pos;
    v3 old_pos;

    // ==== AI ====
    u32 health;

    // Current movement direction
    v2 move_dir;

    // Cooldown until new movement direction is chosen
    f32 move_time;

    // ==== Animation ====
    // Movement speed
    f32 speed;
    f32 wiggle;

    // ==== Graphics ====
    Image *image;
    u32 eye_x;
    u32 eye_y;

    m4 body_mtx;
    m4 sprite_mtx;

    // ==== Other ====
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
    // image_grid(image, WHITE, GRAY);
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

static void monster_update(Monster *mon, f32 dt, Player *player, Random *rng, Gfx *gfx) {
    // ==== Physics ====
    v3 vel = mon->pos - mon->old_pos;
    mon->old_pos = mon->pos;

    // Apply movement
    mon->pos.xz += mon->move_dir * dt;

    mon->move_time -= dt;
    if (mon->move_time <= 0) {
        mon->move_time = rand_f32_range(rng, 2, 10);

        u32 mode = rand_u32_range(rng, 0, 2);
        // fmt_su(OS_FMT, "mode=", mode, "\n");
        if (mode == 0) mon->move_dir = 0;
        if (mode == 1) mon->move_dir = v2_normalize(player->pos.xz - mon->pos.xz) * 0.1;
        if (mode == 2) mon->move_dir = v2_from_rot(rand_f32_signed(rng) * PI) * 0.25;
        monster_set_eyes(mon, rng);
    }

    // Player Collision
    v2 player_dir = player->pos.xz - mon->pos.xz;
    f32 player_distance = v2_length(player_dir);

    float radius = 0.5;
    if (player_distance < radius) {
        mon->pos.xz -= player_dir * (radius - player_distance) / player_distance;
    }

    // ==== Animation ====
    f32 speed = v3_length(vel);
    mon->speed += (speed / dt - mon->speed) * dt;
    mon->wiggle += speed * 2;
    mon->wiggle = f_fract(mon->wiggle);
    mon->body_mtx = m4_billboard(mon->pos, player->pos, f_sin2pi(mon->wiggle) * f_min(mon->speed * 0.5, 0.2) * 0.3);

    mon->sprite_mtx = m4_id();
    m4_translate(&mon->sprite_mtx, (v3){0, 0.5, 0});
    m4_scale(&mon->sprite_mtx, (v3){(f32)mon->image->size.x / 32.0f, (f32)mon->image->size.y / 32.0f, 1});
    m4_apply(&mon->sprite_mtx, mon->body_mtx);

    os_gfx_quad(gfx, mon->sprite_mtx, mon->image, false);
}
