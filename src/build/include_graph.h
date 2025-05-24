// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// include_graph.h - Generate a dependency graph from includes
#pragma once
#include "fmt.h"
#include "fs.h"
#include "mem.h"
#include "read.h"
#include "str_mem.h"

typedef struct Include_Edge Include_Edge;
typedef struct Include_Graph Include_Graph;
typedef struct Include_Node Include_Node;

struct Include_Edge {
    Include_Node *link;
    bool transitive;
    Include_Edge *next;
};

struct Include_Node {
    String name;
    u32 size;
    u32 rank;
    u32 color;
    Include_Edge *edges;
    Include_Node *next;
};

struct Include_Graph {
    Memory *mem;
    u32 rank_count;
    Include_Node *nodes;
    u32 color_ix;
};

static Include_Graph *include_graph_new(Memory *mem) {
    Include_Graph *graph = mem_struct(mem, Include_Graph);
    graph->mem = mem;
    return graph;
}

// Get existing, or insert new node with a given name
static Include_Node *include_graph_node(Include_Graph *graph, String name) {
    // Search for the node
    for (Include_Node *node = graph->nodes; node; node = node->next) {
        if (str_eq(node->name, name)) return node;
    }

    // Insert a new node
    Include_Node *node = mem_struct(graph->mem, Include_Node);
    node->name = name;
    node->next = graph->nodes;
    graph->nodes = node;
    return node;
}

// Append a directed edge from 'src' to 'dst'
static void include_graph_link(Include_Graph *graph, Include_Node *src, Include_Node *dst) {
    Include_Edge *edge = mem_struct(graph->mem, Include_Edge);
    edge->link = dst;
    edge->next = src->edges;
    src->edges = edge;
}

// Write the graph in 'dot' notation
// to the given output stream
static void include_graph_fmt(Include_Graph *graph, Fmt *fmt) {
    fmt_s(fmt, "digraph {\n");
    fmt_s(fmt, "  layout=dot;\n");
    fmt_s(fmt, "  ranksep=1.0;\n");
    fmt_s(fmt, "  node[style=filled,fillcolor=\"#ffffff\",colorscheme=set19];\n");
    fmt_s(fmt, "  edge[color=\"#bbbbbb\"];\n");

    for (u32 i = 0; i < graph->rank_count; ++i) {
        fmt_s(fmt, "  { rank=same;");

        for (Include_Node *node = graph->nodes; node; node = node->next) {
            if (node->rank != i) continue;
            fmt_str(fmt, node->name);
            fmt_s(fmt, "; ");
        }

        fmt_s(fmt, "}\n");
    }

    for (Include_Node *node = graph->nodes; node; node = node->next) {
        fmt_s(fmt, "  ");
        fmt_str(fmt, node->name);
        fmt_s(fmt, "[");
        fmt_s(fmt, "color=");
        fmt_u(fmt, node->color);
        fmt_s(fmt, "];\n");

        for (Include_Edge *edge = node->edges; edge; edge = edge->next) {
            if (edge->transitive) continue;

            fmt_s(fmt, "  ");
            fmt_str(fmt, node->name);
            fmt_s(fmt, " -> ");
            fmt_str(fmt, edge->link->name);

            if (edge->transitive) {
                fmt_s(fmt, "[");
                fmt_s(fmt, "constraint=false,style=dotted");
                fmt_s(fmt, "]");
            };
            fmt_s(fmt, ";\n");
        }
    }
    fmt_s(fmt, "}\n");
}

// Mark edge as transitive
static void include_graph_unlink(Include_Node *src, Include_Node *dst) {
    for (Include_Edge *edge = src->edges; edge; edge = edge->next) {
        if (edge->link == dst) edge->transitive = true;
    }
}

// Mark all transitive links that start at 'src'
static void include_graph_tred_dfs(Include_Node *src, Include_Node *node) {
    for (Include_Edge *edge = node->edges; edge; edge = edge->next) {
        Include_Node *dst = edge->link;
        include_graph_unlink(src, dst);
        if (!edge->transitive) include_graph_tred_dfs(src, dst);
    }
}

// Mark all transitive edges in the graph
static void include_graph_tred(Include_Graph *graph) {
    for (Include_Node *node = graph->nodes; node; node = node->next) {
        for (Include_Edge *edge = node->edges; edge; edge = edge->next) {
            if (edge->transitive) continue;
            include_graph_tred_dfs(node, edge->link);
        }
    }
}

// Create a ranking where all nodes in a given rank only depend on nodes in a lower rank
// For every edge (a, b) -> (a.rank > b.rank)
static void include_graph_rank(Include_Graph *graph) {
    for (;;) {
        bool changed = false;
        for (Include_Node *node = graph->nodes; node; node = node->next) {
            for (Include_Edge *edge = node->edges; edge; edge = edge->next) {
                Include_Node *other = edge->link;

                // Our rank should be at least one higher than our child node's rank
                u32 rank = other->rank + 1;
                if (rank > node->rank) {
                    // A child has a rank equal or higher than our rank,
                    // Our rank should increase
                    node->rank = rank;
                    changed = true;
                }

                // Update total number of ranks (if needed)
                if (rank + 1 > graph->rank_count) {
                    graph->rank_count = rank + 1;
                }
            }
        }

        // Just keep going until a stable configuration has been found
        if (!changed) break;
    }
}

// Append a new node to the graph
// A Include_Node is added for the file
// An edge is added for every '#include'
static Include_Node *include_graph_read_file(Include_Graph *graph, String path, String name) {
    String file = os_readfile(graph->mem, path);
    Include_Node *node = include_graph_node(graph, name);

    u32 line_count = 0;

    String read = file;
    for (;;) {
        String line = read_line(&read);
        if(line.len == 0) break;

        if (!str_drop_start_matching(&line, S("#include \""))) continue;
        if (!str_drop_end_matching(&line, S("\""))) continue;

        // Ignore '../' paths
        if (str_starts_with(line, S("."))) continue;

        // Remove '.c' and '.h'
        str_drop_end_matching(&line, S(".h")) || str_drop_end_matching(&line, S(".c"));

        Include_Node *dst = include_graph_node(graph, line);
        include_graph_link(graph, node, dst);
    }
    node->size = line_count;
    return node;
}

// Add a node for every .h or .c file
static void include_graph_read_dir(Include_Graph *graph, String path) {
    u32 color = graph->color_ix++;

    for (FS_Dir *file = fs_list(graph->mem, path); file; file = file->next) {
        String full_path = str_cat3(graph->mem, path, S("/"), file->name);

        if (file->is_dir) {
            // Recurse
            include_graph_read_dir(graph, full_path);
            continue;
        }

        // Only .c and .h files
        String name_no_ext = file->name;
        bool is_h_file = str_drop_end_matching(&name_no_ext, S(".h"));
        bool is_c_file = str_drop_end_matching(&name_no_ext, S(".c"));
        if (!is_c_file && !is_h_file) continue;

        // Remove extention
        file->name = name_no_ext;

        Include_Node *node = include_graph_read_file(graph, full_path, file->name);
        node->color = color;
    }
}
