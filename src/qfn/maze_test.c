#include "lib/os_main.h"
#include "lib/vec.h"
#include "qfn/maze.h"

static void os_main(void) {
    Fmt *f = G->fmt;
    Maze *maze = maze_new(G->mem, 33, 33);
    maze_init_circle(maze, 1, 0.2);
    maze_generate(maze, G->rand);
    maze_remove_walls(maze, G->rand, 0.2);
    maze_remove_pillars(maze);
    maze_debug_fmt(maze, f);
    os_exit(0);
}
