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
    maze->cells = mem_array_uninit(mem, u8, sx*sy);
    for(u32 y = 0; y < sy; ++y) {
    for(u32 x = 0; x < sx; ++x) {
        Maze_Cell cell = Maze_Cell_Wall;
        f32 dx = 2.0f * ((f32) x + 0.5) / (f32) sx - 1.0f;
        f32 dy = 2.0f * ((f32) y + 0.5) / (f32) sy - 1.0f;
        f32 r = dx*dx + dy*dy;
        if(r > 1.0f) cell = Maze_Cell_Outside;
        maze->cells[y*sx+x] = cell;
    }
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



typedef struct {
    i32 wall_x;
    i32 wall_y;
    i32 cell_x;
    i32 cell_y;
} Maze_Pos;

static Maze_Pos maze_find_suitable_start(Maze *maze) {
    for (u32 y = 1; y < maze->size_y; y += 2) {
        for (u32 x = 1; x < maze->size_x; x += 2) {
            Maze_Cell cell = maze_get(maze, x, y);
            if(cell == Maze_Cell_Wall) return (Maze_Pos){ x, y, x, y };
        }
    }
    assert(false, "No free cells in this maze");
    return (Maze_Pos){};
}

static void maze_generate(Maze *maze, Rand *rng) {
    u32 stack_len = 0;
    Maze_Pos *stack = mem_array_uninit(G->tmp, Maze_Pos, 1024);
    stack[stack_len++] = maze_find_suitable_start(maze);
    
    while(stack_len > 0) {
        u32 ix = rand_u32(rng, 0, stack_len);
        Maze_Pos pos = stack[ix];
        stack[ix] = stack[--stack_len];
        if (maze_get(maze, pos.cell_x, pos.cell_y) != Maze_Cell_Wall) continue;

        maze_set(maze, pos.wall_x, pos.wall_y, Maze_Cell_Inside);
        maze_set(maze, pos.cell_x, pos.cell_y, Maze_Cell_Inside);

        // Add possible directions
        assert0(stack_len + 4 <= 1024);
        stack[stack_len++] = (Maze_Pos){pos.cell_x + 1, pos.cell_y, pos.cell_x + 2, pos.cell_y};
        stack[stack_len++] = (Maze_Pos){pos.cell_x - 1, pos.cell_y, pos.cell_x - 2, pos.cell_y};
        stack[stack_len++] = (Maze_Pos){pos.cell_x, pos.cell_y + 1, pos.cell_x, pos.cell_y + 2};
        stack[stack_len++] = (Maze_Pos){pos.cell_x, pos.cell_y - 1, pos.cell_x, pos.cell_y - 2};
    }
}

static void os_main(void) {
    Fmt *f = G->fmt;
    Maze *maze = maze_new(G->mem, 33, 33);
    maze_generate(maze, G->rand);
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
    os_exit(0);
}
