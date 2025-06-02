// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// level.h - Game level
#pragma once
#include "lib/mem.h"
#include "lib/rand.h"
#include "lib/types.h"
#include "lib/vec.h"
#include "qfn/image.h"
#include "qfn/level_sprite.h"
#include "qfn/maze.h"
#include "qfn/monster.h"
#include "qfn/player.h"

// Ideas
//   Generate in layers, layers give depth
//   Pure white noise is boring, but by layering at different scales sometihng interesting is created.
// Referneces

static void level_add_wall(Memory *mem, Entity **level, Image *img, v3i pos, m4 mtx) {
    v3 pos_f = v3i_to_v3(pos);
    m4_translate(&mtx, pos_f);

    Entity *ent = mem_struct(mem, Entity);
    ent->type = Entity_Wall;
    ent->mtx = mtx;
    ent->image = image_copy(mem, img);
    ent->size = (v2){4, 4};
    ent->next = *level;
    ent->pos = pos_f;
    *level = ent;
}

// Generate an empty level
static void level_generate(Entity **level, Memory *mem, Rand *rng, v2i size) {
    i32 cell_scale = 4;
    f32 adjust = 0.99;

    m4 mtx_yn = m4_id();
    m4_translate_y(&mtx_yn, (f32)cell_scale / 2);
    m4_translate_z(&mtx_yn, -(f32)cell_scale / 2);
    m4_scale(&mtx_yn, adjust);

    m4 mtx_xn = mtx_yn;
    m4 mtx_yp = mtx_yn;
    m4 mtx_xp = mtx_yn;
    m4_rotate_y(&mtx_xn, R1 * 1);
    m4_rotate_y(&mtx_yp, R1 * 2);
    m4_rotate_y(&mtx_xp, R1 * 3);

    // Floor/ceiling
    m4 mtx_zp = m4_id();
    m4_rotate_x(&mtx_zp, R1);
    m4_translate_y(&mtx_zp, cell_scale);
    m4_scale(&mtx_zp, adjust);

    m4 mtx_zn = m4_id();
    m4_rotate_x(&mtx_zn, -R1);
    m4_scale(&mtx_zn, adjust);

    Memory *tmp = mem_new();
    Maze maze = {size, mem_push_uninit(tmp, size.x * size.y)};
    for (i32 y = 0; y < size.y; ++y) {
        for (i32 x = 0; x < size.x; ++x) {
            maze.cell_list[y * size.x + x] = Maze_Cell_Todo;
        }
    }

    // Generate Maze
    maze_generate(&maze, rng);

    Image *wall = level_sprite_generate(mem, rng);
    Image *window = level_sprite_generate(mem, rng);
    Image *floor = level_sprite_generate(mem, rng);
    for (i32 y = -1; y < size.y + 1; ++y) {
        for (i32 x = -1; x < size.x + 1; ++x) {
            v2i pos = {x, y};
            Maze_Cell cell = maze_get(&maze, pos);
            assert(cell != Maze_Cell_Todo, "There are still cells left to-do?");
            if (cell == Maze_Cell_Empty) continue;

            Maze_Cell cell_xp = maze_get(&maze, pos + (v2i){1, 0});
            Maze_Cell cell_xn = maze_get(&maze, pos - (v2i){1, 0});
            Maze_Cell cell_yp = maze_get(&maze, pos + (v2i){0, 1});
            Maze_Cell cell_yn = maze_get(&maze, pos - (v2i){0, 1});

            bool door_xp = cell == Maze_Cell_XP || cell_xp == Maze_Cell_XN;
            bool door_xn = cell == Maze_Cell_XN || cell_xn == Maze_Cell_XP;
            bool door_yp = cell == Maze_Cell_YP || cell_yp == Maze_Cell_YN;
            bool door_yn = cell == Maze_Cell_YN || cell_yn == Maze_Cell_YP;

            // Window if other cell is empty
            bool window_xp = cell_xp == Maze_Cell_Empty;
            bool window_xn = cell_xn == Maze_Cell_Empty;
            bool window_yp = cell_yp == Maze_Cell_Empty;
            bool window_yn = cell_yn == Maze_Cell_Empty;

            v3i wall_pos = (v3i){pos.x, 0, pos.y} * cell_scale;
            if (!door_xp) level_add_wall(mem, level, window_xp ? window : wall, wall_pos, mtx_xp);
            if (!door_xn) level_add_wall(mem, level, window_xn ? window : wall, wall_pos, mtx_xn);
            if (!door_yp) level_add_wall(mem, level, window_yp ? window : wall, wall_pos, mtx_yp);
            if (!door_yn) level_add_wall(mem, level, window_yn ? window : wall, wall_pos, mtx_yn);

            level_add_wall(mem, level, floor, wall_pos, mtx_zn);
            level_add_wall(mem, level, floor, wall_pos, mtx_zp);
        }
    }
    mem_free(tmp);
}
