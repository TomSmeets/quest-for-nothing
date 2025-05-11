// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs_linux.h: Platform independent filesystem
#pragma once
#include "fs_types.h"
#include "mem.h"
#include "os_api.h"
#include "str_mem.h"

static void fs_mkdir(char *path) {
    linux_mkdir(path, 0777);
}

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
            String name = str_from(ent->name);

            bool is_hidden = str_starts_with(name, S("."));
            if (is_hidden) continue;

            FS_Dir *info = mem_struct(mem, FS_Dir);
            info->name = str_clone(mem, name);
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
