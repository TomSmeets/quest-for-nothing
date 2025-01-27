#pragma once
#if OS_IS_LINUX
#include "math.h"
#include "mem.h"
#include "os_impl.h"
#include "read.h"

typedef struct File_Info File_Info;
struct File_Info {
    char *name;
    bool is_dir;
    File_Info *next;
};

static File_Info *os_read_dir(char *path, Memory *mem) {
    i32 dir = linux_open(path, O_RDONLY | O_DIRECTORY, 0);
    if (dir < 0) return 0;

    File_Info *first = 0;
    File_Info *last = 0;

    // Allocate temp buffer (it is cached)
    void *buffer = mem_alloc_chunk();

    for (;;) {
        i64 len = linux_getdents64(dir, buffer, MEMORY_CHUNK_SIZE);

        // Some Error occured
        if (len < 0) {
            first = last = 0;
            break;
        }

        // Should not happen
        assert(len <= MEMORY_CHUNK_SIZE, "getdents64 returned too many bytes");

        // End of directory
        if (len == 0) break;

        for (struct linux_dirent64 *ent = buffer; (void *)ent < buffer + len; ent = (void *)ent + ent->reclen) {
            File_Info *info = mem_struct(mem, File_Info);
            info->name = str_dup(ent->name, mem);
            info->is_dir = ent->type == DT_DIR;
            LIST_APPEND(first, last, info);
        }
    }

    // Release buffer back to the cache
    mem_free_chunk(buffer);
    linux_close(dir);
    return first;
}

static char *fmt_mem_ss(Memory *mem, char *s0, char *s1, char *s2) {
    Fmt *fmt = fmt_memory(mem);
    fmt_ss(fmt, s0, s1, s2);
    return fmt_close(fmt);
}

// Generate Include Dot Graph
static void include_graph(void) {
    char *path = "src";

    Memory *mem = mem_new();
    Fmt *out = fmt_open(mem, "out/include-graph.dot");
    fmt_s(out, "digraph {\n");
    fmt_s(out, "  layout=dot;\n");
    fmt_s(out, "  node[style=filled,fillcolor=\"#ffffff\"];\n");
    fmt_s(out, "  edge[color=\"#bbbbbb\"];\n");

    for (File_Info *file = os_read_dir(path, mem); file; file = file->next) {
        // Skip '.', '..', and hidden files
        bool is_hidden = str_starts_with(file->name, ".");
        if (is_hidden) continue;

        // Only .c and .h files
        bool is_h_file = str_ends_with(file->name, ".h");
        bool is_c_file = str_ends_with(file->name, ".c");
        if (!is_c_file && !is_h_file) continue;

        // Don't scan opengl api, it is quite big.
        if (str_eq(file->name, "ogl_api.h")) continue;

        // Full Path
        char *full_path = fmt_mem_ss(mem, path, "/", file->name);

        // Read file
        Read *read = read_new(mem, full_path);

        // Remove extention
        if (is_c_file || is_h_file) {
            file->name[str_len(file->name) - 2] = 0;
        }

        u32 line_count = 0;
        u32 dep_count = 0;
        for (;;) {
            char buffer[1024];
            char *line = read_line(read, buffer, sizeof(buffer));
            if (!line) break;
            line_count++;

            char *prefix = "#include \"";
            char *suffix = "\"";
            if (!str_starts_with(line, prefix)) continue;
            if (!str_ends_with(line, suffix)) continue;

            u32 len = str_len(line);
            line[len - str_len(suffix)] = 0;
            line += str_len(prefix);

            // Ignore '../' paths
            if (line[0] == '.') continue;

            // Remove '.c' and '.h'
            if (str_ends_with(line, ".h") || str_ends_with(line, ".c")) {
                line[str_len(line) - 2] = 0;
            }

            dep_count++;
            fmt_sss(out, "  ", file->name, " -> ", line, ";\n");
        }
        fmt_ss(out, "  ", file->name, "[");
        f32 size = (f32)f_sqrt(line_count) * 2;
        if (size < 14) size = 14;
        fmt_sf(out, "fontsize=", size, ",");
        if (is_c_file)
            fmt_s(out, "fillcolor=\"#ffbbbb\",");
        else if (dep_count == 0)
            fmt_s(out, "fillcolor=\"#eeeeff\",");
        fmt_s(out, "];\n");
        read_close(read);
    }
    fmt_s(out, "}\n");
    fmt_close(out);
    mem_free(mem);
    os_system("tred out/include-graph.dot > out/include-graph-reduced.dot");
}
#else
static void include_graph(void) {
    os_fail("Not supported on this plaform");
}
#endif
