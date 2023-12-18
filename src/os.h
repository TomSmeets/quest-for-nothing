// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os.h - api for platform implementations
#pragma once
#include "inc.h"

// ==== Basics ====
static void os_print(char *out);
static void os_assert(bool assertion, char *message);

// Get the current time in micro seconds
static u64 os_time(void);

// Sleep for some micro seconds
static void os_sleep(u64);

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
