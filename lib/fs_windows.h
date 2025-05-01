// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs_windows.h: Platform independent filesystem
#pragma once
#include "fs_types.h"
#include "mem.h"
#include "os_api.h"
#include "str_mem.h"

static void fs_mkdir(char *path) {
    CreateDirectoryA(path, NULL);
}

static void fs_remove(char *path) {
    DeleteFileA(path);
}

static FS_Dir *fs_list(Memory *mem, char *path) {
    FS_Dir *first = 0;
    FS_Dir *last = 0;

    char *search_path = str_cat3(mem, "", path, "\\*");

    WIN32_FIND_DATAA find_data;
    HANDLE handle = FindFirstFileA(search_path, &find_data);
    if (handle == INVALID_HANDLE_VALUE) return 0;

    do {
        // Skip "." and ".."
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;

        FS_Dir *info = mem_struct(mem, FS_Dir);
        info->name = str_dup(find_data.cFileName, mem);
        info->is_dir = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        LIST_APPEND(first, last, info);
    } while (FindNextFileA(handle, &find_data));
    FindClose(handle);
    return first;
}
