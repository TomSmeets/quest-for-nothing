// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs_windows.h: Platform independent filesystem
#pragma once
#include "lib/fs_api.h"
#include "lib/os_api.h"
#include "lib/mem.h"
#include "lib/str_mem.h"
#include "lib/fmt.h"

static File *fs_open(String path, FS_Open_Type type) {
    return 0;
}

static void fs_close(File *file) {
}

static bool fs_remove(String path) {
    CString cstr;
    return DeleteFileA(str_c(&cstr, path));
}

static bool fs_mkdir(String path) {
    CString cstr;
    return CreateDirectoryA(str_c(&cstr, path), NULL);
}


static FS_Dir *fs_list(Memory *mem, String path) {
    CString cstr;
    FS_Dir *first = 0;
    FS_Dir *last = 0;

    String search_path = str_cat3(mem, S0, path, S("\\*"));


    // fmt always includes zero terminator
    // TODO: maybe go back to format strings
    // String search_path = fmt(mem, "%s\\*", path);
    // assert(search_path.zero_terminated, "Fmt always includes zero terminator");
    // Or is this nicer?
    // Fmt *search_path = fmt_new(mem, 0);
    // fmt_str(search_path, path);
    // fmt_str(search_path, S("\\*"));
    // fmt_zero(search_path);

    WIN32_FIND_DATAA find_data;
    HANDLE handle = FindFirstFileA(str_c(&cstr, search_path), &find_data);
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
