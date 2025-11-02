// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// level.h - Game level
#pragma once
#include "gfx/image.h"
#include "lib/mem.h"
#include "lib/rand.h"
#include "lib/types.h"
#include "lib/vec.h"
#include "qfn/level_sprite.h"
#include "qfn/maze.h"
#include "qfn/monster.h"
#include "qfn/player.h"
#include "qfn/wall.h"

TYPEDEF_STRUCT(Level2);
struct Level2 {
    Maze *maze;
    Wall *walls;
    v3i spawn;
};

// Ideas
//   Generate in layers, layers give depth
//   Pure white noise is boring, but by layering at different scales sometihng interesting is created.
// Referneces

static void level_add_wall(Memory *mem, Level2 *level, Image *img, v3i pos, m4 mtx) {
    v3 pos_f = v3i_to_v3(pos);
    m4_translate(&mtx, pos_f);

    Wall *ent = wall_new(mem, mtx, img);
    LIST_PUSH(level->walls, ent);
}

// Generate an empty level
static Level2 *level_generate(Memory *mem, Rand *rng, v2i size) {
    Level2 *level = mem_struct(mem, Level2);

    i32 cell_scale = 2;
    f32 adjust = 1.0;

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

    // Generate maze
    u32 maze_sx = size.x*2+1;
    u32 maze_sy = size.y*2+1;
    Maze *maze = maze_new(G->tmp, maze_sx, maze_sy);
    maze_init_circle(maze, 1.0, 0.3);
    maze_generate(maze, rng);
    maze_remove_walls(maze, rng, 0.2);
    maze_remove_pillars(maze);
    level->maze = maze;

    Image *wall = level_sprite_generate(mem, rng);
    Image *window = level_sprite_generate(mem, rng);
    Image *floor = level_sprite_generate(mem, rng);

    for (u32 y = 0; y < window->size.y; y++) {
        for (u32 x = 0; x < window->size.x; x++) {
            f32 dx = ((f32) x + 0.5) / window->size.x * 2 - 1;
            f32 dy = ((f32) y + 0.5) / window->size.y * 2 - 1;
            f32 r = dx*dx+dy*dy;
            if(r > 0.5f*0.5f) continue;
            v4 color = { 0.5, 0.7, 1.0, 1 };
            if(r < 0.48f*0.48f) color.w = 0.3;
            image_write4(window, (v2i){x, y}, color);
        }
    }

    for (i32 y = 1; y < maze_sx; y += 2) {
        for (i32 x = 1; x < maze_sy; x += 2) {
            Maze_Cell cell = maze_get(maze, x, y);
            if(cell != Maze_Cell_Inside) continue;

            Maze_Cell wall_xp = maze_get(maze, x + 1, y);
            Maze_Cell wall_xn = maze_get(maze, x - 1, y);
            Maze_Cell wall_yp = maze_get(maze, x, y + 1);
            Maze_Cell wall_yn = maze_get(maze, x, y - 1);

            Maze_Cell cell_xp = maze_get(maze, x + 2, y);
            Maze_Cell cell_xn = maze_get(maze, x - 2, y);
            Maze_Cell cell_yp = maze_get(maze, x, y + 2);
            Maze_Cell cell_yn = maze_get(maze, x, y - 2);

            bool door_xp = wall_xp == Maze_Cell_Inside;
            bool door_xn = wall_xn == Maze_Cell_Inside;
            bool door_yp = wall_yp == Maze_Cell_Inside;
            bool door_yn = wall_yn == Maze_Cell_Inside;

            // Window if other cell is empty
            bool window_xp = cell_xp == Maze_Cell_Outside && rand_choice(rng, 0.5);
            bool window_xn = cell_xn == Maze_Cell_Outside && rand_choice(rng, 0.5);
            bool window_yp = cell_yp == Maze_Cell_Outside && rand_choice(rng, 0.5);
            bool window_yn = cell_yn == Maze_Cell_Outside && rand_choice(rng, 0.5);

            v3i wall_pos = (v3i){(x - 1) / 2, 0, (y - 1) / 2} * cell_scale;
            level->spawn = wall_pos;
            if (!door_xp) level_add_wall(mem, level, window_xp ? window : wall, wall_pos, mtx_xp);
            if (!door_xn) level_add_wall(mem, level, window_xn ? window : wall, wall_pos, mtx_xn);
            if (!door_yp) level_add_wall(mem, level, window_yp ? window : wall, wall_pos, mtx_yp);
            if (!door_yn) level_add_wall(mem, level, window_yn ? window : wall, wall_pos, mtx_yn);

            level_add_wall(mem, level, floor, wall_pos, mtx_zn);
            level_add_wall(mem, level, floor, wall_pos, mtx_zp);
        }
    }
    return level;
}
