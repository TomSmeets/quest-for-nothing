#pragma once
#include "fs.h"

static void tmpfs_clear(char *dir) {
    Memory *tmp = mem_new();
    for (FS_Dir *file = fs_list(tmp, dir); file; file = file->next) {
        if (file->is_dir) continue;
        if (!str_starts_with(file->name, "tmp-")) continue;
        char *path = str_cat3(tmp, dir, "/", file->name);
        fmt_ss(G->fmt, "tmpfs remove: ", path, "\n");
        fs_remove(path);
    }
    mem_free(tmp);
}

// Get a new absolute path to a temporary file
static char *tmpfs_path(Memory *mem, char *dir, char *name) {
    Fmt *f = fmt_memory(mem);
    fmt_s(f, dir);
    fmt_s(f, "/");
    fmt_s(f, "tmp-");
    fmt_u(f, rand_next(G->rand));
    fmt_s(f, "-");
    fmt_s(f, name);
    char *path = fmt_close(f);
    return path;
}
