// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sparse.h - Incremental BVH
#pragma once
#include "box.h"
#include "mem.h"

typedef struct Sparse Sparse;
typedef struct Sparse_Node Sparse_Node;
typedef struct Sparse_Cell Sparse_Cell;
typedef struct Sparse_Collision Sparse_Collision;

#define SPARSE_BOX_SIZE 4

struct Sparse_Collision {
    Sparse_Node *node;
    Sparse_Collision *next;
};

struct Sparse_Node {
    Box box;
    void *user;
    Sparse_Node *next;
};

// A big cell, contianing multiple nodes
struct Sparse_Cell {
    Box box;

    // Cell pos
    v3i pos;

    // Nodes in this Cell
    Sparse_Node *nodes;
    Sparse_Cell *next;
};

struct Sparse {
    Memory *mem;
    Sparse_Cell *cells[32 * 32];
};

static u32 _sparse_hash(v3i pos) {
    return (pos.x % 32) + (pos.z % 32) * 32;
}

static Sparse *sparse_new(Memory *mem) {
    Sparse *sparse = mem_struct(mem, Sparse);
    sparse->mem = mem;
    return sparse;
}

static void sparse_add(Sparse *sparse, Box box, void *user) {
    v3i pos = v3_to_v3i(box_center(box) * 1.0f / SPARSE_BOX_SIZE + (v3){0.5, 0.0, 0.5});
    pos.y = 0;

    Sparse_Cell **cell_list = sparse->cells + _sparse_hash(pos);

    // Find Cell at pos
    Sparse_Cell *cell = *cell_list;
    for (;;) {
        if (!cell) break;
        if (v3i_eq(cell->pos, pos)) break;
        cell = cell->next;
    }

    if (!cell) {
        // No cell found, create a new one
        cell = mem_struct(sparse->mem, Sparse_Cell);
        cell->pos = pos;
        cell->box = box;
        cell->next = *cell_list;
        *cell_list = cell;
    } else {
        // Cell exists, grow box
        cell->box = box_union(cell->box, box);
    }

    // Add node to cell
    Sparse_Node *node = mem_struct(sparse->mem, Sparse_Node);
    node->box = box;
    node->user = user;
    node->next = cell->nodes;
    cell->nodes = node;
}

static Sparse_Collision *sparse_check(Sparse *sparse, Box box) {
    Sparse_Collision *col_list = 0;
    for (u32 i = 0; i < array_count(sparse->cells); ++i) {
        for (Sparse_Cell *cell = sparse->cells[i]; cell; cell = cell->next) {
            if (!box_intersect(cell->box, box)) continue;
            for (Sparse_Node *node = cell->nodes; node; node = node->next) {
                if (!box_intersect(node->box, box)) continue;
                Sparse_Collision *col = mem_struct(sparse->mem, Sparse_Collision);
                col->next = col_list;
                col->node = node;
                col_list = col;
            }
        }
    }
    return col_list;
}
