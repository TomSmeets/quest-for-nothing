// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// maze.h - Generic Maze generator
#pragma once
#include "lib/os_main.h"
#include "lib/vec.h"

TYPEDEF_STRUCT(Maze);

typedef enum {
    Maze_Cell_Outside,
    Maze_Cell_Inside,
    Maze_Cell_Wall,
} Maze_Cell;

struct Maze {
    u32 size_x;
    u32 size_y;
    u8 *cells;
};

static Maze *maze_new(Memory *mem, u32 sx, u32 sy) {
    assert(sx % 2 == 1, "Size must be odd");
    assert(sy % 2 == 1, "Size must be odd");

    Maze *maze = mem_struct(mem, Maze);
    maze->size_x = sx;
    maze->size_y = sy;
    maze->cells = mem_array_zero(mem, u8, sx*sy);
    for (u32 i = 0; i < maze->size_x * maze->size_y; ++i) {
        maze->cells[i] = Maze_Cell_Wall;
    }
    return maze;
}

static Maze_Cell maze_get(Maze *maze, i32 x, i32 y) {
    if(x < 0)             return Maze_Cell_Outside;
    if(y < 0)             return Maze_Cell_Outside;
    if(x >= maze->size_x) return Maze_Cell_Outside;
    if (x >= maze->size_y) return Maze_Cell_Outside;
    return maze->cells[y*maze->size_x + x];
}

static void maze_set(Maze *maze, i32 x, i32 y, Maze_Cell value) {
    if(x < 0)             return;
    if(y < 0)             return;
    if(x >= maze->size_x) return;
    if (x >= maze->size_y) return;
    maze->cells[y*maze->size_x + x] = value;
}

static void maze_init_circle(Maze *maze, f32 r1, f32 r2) {
    for (u32 y = 0; y < maze->size_y; ++y) {
        for (u32 x = 0; x < maze->size_x; ++x) {
            Maze_Cell cell = Maze_Cell_Wall;
            f32 dx = 2.0f * ((f32)x + 0.5) / (f32)maze->size_x - 1.0f;
            f32 dy = 2.0f * ((f32)y + 0.5) / (f32)maze->size_x - 1.0f;
            f32 r = dx * dx + dy * dy;
            if (r > r1*r1) cell = Maze_Cell_Outside;
            if (r < r2*r2) cell = Maze_Cell_Outside;
            maze_set(maze, x, y, cell);
        }
    }
}



typedef struct {
    i32 wall_x;
    i32 wall_y;
    i32 cell_x;
    i32 cell_y;
} Maze_Pos;

static Maze_Pos maze_find_first(Maze *maze, Maze_Cell type) {
    for (u32 y = 1; y < maze->size_y; y += 2) {
        for (u32 x = 1; x < maze->size_x; x += 2) {
            Maze_Cell cell = maze_get(maze, x, y);
            if(cell == type) return (Maze_Pos){ x, y, x, y };
        }
    }
    assert(false, "No free cells in this maze");
    return (Maze_Pos){};
}

static void maze_generate(Maze *maze, Rand *rng) {
    u32 stack_max = 1024;
    u32 stack_len = 0;
    Maze_Pos *stack = mem_array_uninit(G->tmp, Maze_Pos, stack_max);
    stack[stack_len++] = maze_find_first(maze, Maze_Cell_Wall);
    
    while(stack_len > 0) {
        // Pop a random item from the stack
        u32 ix = rand_u32(rng, 0, stack_len);
        Maze_Pos pos = stack[ix];
        stack[ix] = stack[--stack_len];

        // Wall from previous position
        Maze_Cell c_wall = maze_get(maze, pos.wall_x, pos.wall_y);

        // Cell in next position
        Maze_Cell c_cell = maze_get(maze, pos.cell_x, pos.cell_y);
        // if(c_cell == Maze_Cell_Outside) maze_set(maze, pos.wall_x, pos.wall_y, Maze_Cell_Wall);

        // Ensure the next position is valid
        if (c_wall != Maze_Cell_Wall) continue;
        if (c_cell != Maze_Cell_Wall) continue;

        // Carve a path to the new position
        maze_set(maze, pos.wall_x, pos.wall_y, Maze_Cell_Inside);
        maze_set(maze, pos.cell_x, pos.cell_y, Maze_Cell_Inside);

        // Add possible future directions
        assert0(stack_len + 4 <= stack_max);
        stack[stack_len++] = (Maze_Pos){pos.cell_x + 1, pos.cell_y, pos.cell_x + 2, pos.cell_y};
        stack[stack_len++] = (Maze_Pos){pos.cell_x - 1, pos.cell_y, pos.cell_x - 2, pos.cell_y};
        stack[stack_len++] = (Maze_Pos){pos.cell_x, pos.cell_y + 1, pos.cell_x, pos.cell_y + 2};
        stack[stack_len++] = (Maze_Pos){pos.cell_x, pos.cell_y - 1, pos.cell_x, pos.cell_y - 2};
    }
}

static bool maze_remove_wall(Maze *maze, i32 x, i32 y, u32 direction) {
    assert0(direction < 4);

    Maze_Cell src = maze_get(maze, x, y);
    if(src != Maze_Cell_Inside) return false;

    u32 dx = 0;
    u32 dy = 0;
    if(direction == 0) dx+=1;
    if(direction == 1) dx-=1;
    if(direction == 2) dy+=1;
    if(direction == 3) dy-=1;

    Maze_Cell wall = maze_get(maze, x + dx, y + dy);
    Maze_Cell cell = maze_get(maze, x + dx*2, y + dy*2);
    if(wall != Maze_Cell_Wall)   return false;
    if(cell != Maze_Cell_Inside) return false;
    maze_set(maze, x + dx, y + dy, Maze_Cell_Inside);
    return true;
}

static void maze_remove_walls(Maze *maze, Rand *rng, f32 p) {
    for (u32 i = 0; i < maze->size_x*maze->size_y*p; ++i) {
        u32 x = rand_i32(rng, 0, (maze->size_x - 1) / 2) * 2 + 1;
        u32 y = rand_i32(rng, 0, (maze->size_y - 1) / 2) * 2 + 1;
        u32 dir = rand_u32(rng, 0, 4);
        maze_remove_wall(maze, x, y, dir);
    }
}

static void maze_remove_pillars(Maze *maze) {
    for (u32 y = 0; y < maze->size_y; y += 2) {
        for (u32 x = 0; x < maze->size_x; x += 2) {
            if(maze_get(maze, x, y) != Maze_Cell_Wall) continue;;

            Maze_Cell c1 = maze_get(maze, x+1, y);
            Maze_Cell c2 = maze_get(maze, x-1, y);
            Maze_Cell c3 = maze_get(maze, x,   y+1);
            Maze_Cell c4 = maze_get(maze, x,   y-1);
            if(c1 != Maze_Cell_Inside) continue;
            if(c2 != Maze_Cell_Inside) continue;
            if(c3 != Maze_Cell_Inside) continue;
            if(c4 != Maze_Cell_Inside) continue;
            maze_set(maze, x, y, Maze_Cell_Inside);
        }
    }
}

// Format maze
static void maze_debug_fmt(Maze *maze, Fmt *f) {
    for(u32 y = 0; y < maze->size_y; ++y) {
        for (u32 x = 0; x < maze->size_x; ++x) {
            Maze_Cell cell = maze_get(maze, x, y);
            if (cell == Maze_Cell_Inside) fmt_s(f, " ");
            if (cell == Maze_Cell_Wall) fmt_s(f, "#");
            if (cell == Maze_Cell_Outside) fmt_s(f, "~");
             fmt_s(f, " ");
        }
        fmt_s(f, "\n");
    }
}

