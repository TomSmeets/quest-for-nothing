// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// maze.h - Generic Maze generator
#pragma once
#include "rand.h"
#include "types.h"

typedef enum {
    // Outsdie the maze, don't generate here
    Maze_Cell_Empty,

    // Inside the maze, not yet generated
    Maze_Cell_Todo,

    // Cell is generated and has a door in the given direction
    Maze_Cell_Start,
    Maze_Cell_XN,
    Maze_Cell_XP,
    Maze_Cell_YN,
    Maze_Cell_YP,
} Maze_Cell;

typedef struct {
    v2i size;
    Maze_Cell *cell_list;
} Maze;

static Maze_Cell *maze_ref(Maze *maze, v2i pos) {
    if (pos.x < 0 || pos.x >= maze->size.x) return 0;
    if (pos.y < 0 || pos.y >= maze->size.y) return 0;
    return maze->cell_list + pos.y * maze->size.x + pos.x;
}

static Maze_Cell maze_get(Maze *maze, v2i pos) {
    Maze_Cell *cell = maze_ref(maze, pos);
    if (!cell) return Maze_Cell_Empty;
    return *cell;
}

// Remove a random element from the list
static Maze_Cell rand_choose(Rand *rng, Maze_Cell *list, u32 *len) {
    assert(*len > 0, "No more elments to choose from!");
    u32 i = rand_u32(rng, 0, *len);
    Maze_Cell value = list[i];
    list[i] = list[--*len];
    return value;
}

static v2i maze_dir_to_v2i(Maze_Cell dir) {
    if (dir == Maze_Cell_XN) return (v2i){-1, 0};
    if (dir == Maze_Cell_XP) return (v2i){+1, 0};
    if (dir == Maze_Cell_YN) return (v2i){0, -1};
    if (dir == Maze_Cell_YP) return (v2i){0, +1};
    return (v2i){0, 0};
}

// Continue generation at a given node
static void maze_generate_at(Maze *maze, Rand *rng, v2i pos) {
    // Possible directions
    Maze_Cell open_list[] = {
        Maze_Cell_XN,
        Maze_Cell_XP,
        Maze_Cell_YN,
        Maze_Cell_YP,
    };
    u32 open_count = array_count(open_list);

    for (;;) {
        // No more directions left, time to backtrace
        if (open_count == 0) break;

        // Choose a random next direction
        // This direction points from next to our cell
        Maze_Cell next_dir = rand_choose(rng, open_list, &open_count);

        // Next position
        v2i next_pos = pos - maze_dir_to_v2i(next_dir);
        Maze_Cell *next = maze_ref(maze, next_pos);

        // Cell does not exist
        if (!next) continue;

        // Cell is already generated or empty
        if (*next != Maze_Cell_Todo) continue;

        // Make cell point to us
        *next = next_dir;

        // Continue from next cell
        maze_generate_at(maze, rng, next_pos);
    }
}

// Generate a maze
static void maze_generate(Maze *maze, Rand *rng) {
    v2i start_pos = {0, 0};
    *maze_ref(maze, start_pos) = Maze_Cell_Start;
    maze_generate_at(maze, rng, start_pos);
}
