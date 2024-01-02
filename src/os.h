// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os.h - api for platform implementations
#pragma once
#include "inc.h"

// ==== Main Entrypoint ====
// These are called by the os
// Use os_exit to quit
void *main_init(int argc, char **argv);
void main_update(void *handle);

// ==== Basics ====
static void os_print(char *out);
static void os_error(char *out);
static void os_exit(u32 code);

// Get the current time in micro seconds
static u64 os_time(void);

// Sleep until the given time
static void os_sleep_until(u64 time);

// ==== Memory ====
struct mem_page {
    // total size of this memory page (including this struct)
    u64 size;
    // next page in the list
    mem_page *next;
    // ... the rest is the free memory
};

// We need some method to alloc and free memory
// size is the minimum free space of this page.
static mem_page *os_alloc_page(u64 size);
static void os_free_page(mem_page *page);

// ==== Filesystem ====
struct os_dir {
    char *file_name;
    bool is_file;
    bool is_dir;
    bool is_link;
    os_dir *next;
};

static void os_copy_file(char *src, char *dst);
static buf os_read_file(mem *m, char *path);
static u64 os_file_mtime(char *path);
static os_dir *os_read_dir(mem *m, char *path);

// ==== Dynamic Libraries ====
// optional, returns null if unsoported
static void *os_dlopen(char *path);
static void *os_dlsym(void *handle, char *sym);

// ==== Processes ====

// Run a system command in the system shell
// returns 0 on success, and anything else on error
static i32 os_command(char *command);
