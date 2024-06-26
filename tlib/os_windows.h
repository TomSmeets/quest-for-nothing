// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_windows.h - Windows API wrapper and platform implementation
#pragma once
#include "tlib/fmt.h"
#include "tlib/inc.h"
#include "tlib/mem.h"
#include "tlib/os.h"
#include "tlib/os_mem_caching.h"

#include <stdio.h>
#include <windows.h>

// ==== Main Entrypoint ====
// int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_inst_prev, PWSTR cmdline, int n_cmd_show);
int main(int argc, char *argv[]) {
    void *app = main_init(argc, argv);
    for (;;) {
        main_update(app);
    }
}

// ==== Basics ====
static void os_print(char *msg) {
    HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
    assert(con != INVALID_HANDLE_VALUE);
    assert(WriteFile(con, msg, str_len(msg), 0, 0));
}

static void os_error(char *msg) {
    HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(con, msg, str_len(msg), 0, 0);

    // check windows error
    DWORD last_error = GetLastError();
    char *last_error_msg = 0;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, last_error, 0,
        (LPTSTR)&last_error_msg, 0, 0
    );
    WriteFile(con, last_error_msg, str_len(last_error_msg), 0, 0);
    os_exit(1);
}

static void os_exit(u32 code) {
    ExitProcess(code);
}

static u64 os_time(void) {
    LARGE_INTEGER big_freq, big_count;
    assert(QueryPerformanceFrequency(&big_freq));
    assert(QueryPerformanceCounter(&big_count));
    i64 freq = big_freq.QuadPart;
    i64 count = big_count.QuadPart;
    assert(freq >= 1000 * 1000);
    assert(count >= 0);
    return (u64)count / ((u64)freq / 1000 / 1000);
}

static void os_sleep_until(u64 target_time) {
    for (;;) {
        u64 time = os_time();
        if (time > target_time)
            break;

        i64 sleep_time = (i64)(target_time - time) * .5 - 1000;
        if (sleep_time > 0)
            Sleep(sleep_time / 1000);
    }
}

// ==== Memory ====
static mem_page *os_alloc_page_uncached(u64 size) {
    mem_page *page = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(page);
    page->size = size;
    page->next = 0;
    return page;
}

static void os_free_page_uncached(mem_page *page) {
    assert(page);
    assert(VirtualFree(page, 0, MEM_RELEASE));
}

static os_dir *os_read_dir(mem *m, char *path) {
    os_dir *first = 0;
    os_dir *last = 0;

    char *query = fmt(m, "%s\\*", path);

    WIN32_FIND_DATA ent;
    HANDLE c_dir = FindFirstFile(query, &ent);
    assert(c_dir != INVALID_HANDLE_VALUE);
    do {
        char *file_name = ent.cFileName;

        // skip these
        if (str_eq(file_name, "."))
            continue;
        if (str_eq(file_name, ".."))
            continue;

        os_dir *dir = mem_struct(m, os_dir);
        dir->file_name = str_dup(m, file_name);
        dir->is_dir = (ent.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        dir->is_file = !dir->is_dir;

        // append to the list
        if (!last) {
            first = dir;
            last = dir;
        } else {
            last->next = dir;
            last = dir;
        }
    } while (FindNextFile(c_dir, &ent) != 0);
    FindClose(c_dir);

    return first;
}

static Buffer os_read_file(mem *m, char *path) {
    HANDLE fd =
        CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    assert(fd != INVALID_HANDLE_VALUE);

    u64 size = GetFileSize(fd, 0);
    assert(size != INVALID_FILE_SIZE);

    u8 *buffer = mem_push(m, size + 1);

    DWORD bytes_read = 0;
    assert(ReadFile(fd, buffer, size, &bytes_read, 0));
    assert((u64)bytes_read == size);
    buffer[size] = 0;

    CloseHandle(fd);

    return (Buffer){.ptr = buffer, .size = size};
}

static i32 os_command(char *cmd) {
    return system(cmd);
}

static void *os_dlopen(char *file) {
    return LoadLibrary(file);
}

static void *os_dlsym(void *handle, char *name) {
    void *addr = GetProcAddress(handle, name);
    assert(addr);
    return addr;
}

static void os_copy_file(char *src, char *dst) {
    assert(CopyFile(src, dst, 0));
}

static u64 os_file_mtime(char *path) {
    HANDLE fd =
        CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    assert(fd != INVALID_HANDLE_VALUE);

    FILETIME ft_create, ft_access, ft_write;
    GetFileTime(fd, &ft_create, &ft_access, &ft_write);

    ULARGE_INTEGER uli;
    uli.LowPart = ft_write.dwLowDateTime;
    uli.HighPart = ft_write.dwHighDateTime;
    u64 time = uli.QuadPart / 10;
    CloseHandle(fd);
    return time;
}
