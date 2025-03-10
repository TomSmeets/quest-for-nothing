// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs.h: Platform independent filesystem
#pragma once
#include "mem.h"
#include "str_mem.h"

typedef struct FS_Dir {
    char *name;
    bool is_dir;
    struct FS_Dir *next;
} FS_Dir;

static FS_Dir *fs_list(struct Memory *mem, char *path);
static void fs_remove(char *path);
static void fs_mkdir(char *path);

#if OS_IS_LINUX
#include "linux_api.h"

static void fs_remove(char *path) {
    linux_unlink(path);
}

static FS_Dir *fs_list(Memory *mem, char *path) {
    i32 dir = linux_open(path, O_RDONLY | O_DIRECTORY, 0);
    if (dir < 0) return 0;

    FS_Dir *first = 0;
    FS_Dir *last = 0;

    // Allocate temp buffer (it is cached)
    Chunk *chunk = (void *)chunk_alloc();
    void *buffer = (void *)chunk;

    for (;;) {
        i64 len = linux_getdents64(dir, buffer, CHUNK_SIZE);

        // Some Error occured
        if (len < 0) {
            first = last = 0;
            break;
        }

        // Should not happen
        assert(len <= CHUNK_SIZE, "getdents64 returned too many bytes");

        // End of directory
        if (len == 0) break;

        for (struct linux_dirent64 *ent = buffer; (void *)ent < buffer + len; ent = (void *)ent + ent->reclen) {
            bool is_hidden = str_starts_with(ent->name, ".");
            if (is_hidden) continue;

            FS_Dir *info = mem_struct(mem, FS_Dir);
            info->name = str_dup(ent->name, mem);
            info->is_dir = ent->type == DT_DIR;
            LIST_APPEND(first, last, info);
        }
    }

    // Restore the chunk header
    chunk->next = 0;

    // Release the chunk
    chunk_free(chunk);

    linux_close(dir);
    return first;
}

static void fs_mkdir(char *path) {
    linux_mkdir(path, 0777);
}
#endif
