// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs_linux.h: Platform independent filesystem
#pragma once
#include "lib/fs_api.h"
#include "lib/mem.h"
#include "lib/os_api.h"
#include "lib/str_mem.h"

static File *fs_open(String path, FS_Open_Type type) {
    return 0;
}

static void fs_close(File *file) {
}

static bool fs_remove(String path) {
    return linux_unlink(str_c(path)) == 0;
}

static bool fs_mkdir(String path) {
    return linux_mkdir(str_c(path), 0777) == 0;
}

static bool fs_rmdir(String path) {
    return linux_rmdir(str_c(path)) == 0;
}

static FS_Dir *fs_list(Memory *mem, String path) {
    i32 dir = linux_open(str_c(path), O_RDONLY | O_DIRECTORY, 0);
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
