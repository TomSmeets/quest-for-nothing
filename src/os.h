// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os.h - Base platform API
#pragma once
#include "types.h"

#if __unix__
#define OS_IS_LINUX 1
#else
#define OS_IS_LINUX 0
#endif

#if _WIN32
#define OS_IS_WINDOWS 1
#else
#define OS_IS_WINDOWS 0
#endif

#if __wasm__
#define OS_IS_WASM 1
#else
#define OS_IS_WASM 0
#endif

typedef struct OS OS;
struct OS {
    // Command line args
    u32 argc;
    char **argv;

    // Was the application reloaded
    bool reloaded;

    // Time to sleep until next call
    u64 sleep_time;
};

typedef struct File File;

// Callbacks
static void os_main(void);

// Read
static u64 os_time(void);

// Actions
static void os_write(File *file, u8 *data, u32 len);
static void os_exit(i32 code);
static void os_fail(char *message);
static void *os_alloc_raw(u32 size);

// === Desktop API ===
typedef enum {
    Open_Write,
    Open_Read,
    Open_Dir,
} OS_Open_Type;

static File *os_open(char *path, OS_Open_Type type);
static void os_close(File *file);

static u32 os_read(File *file, u8 *data, u32 len);
static void os_sleep(u64 time);

static File *os_dlopen(char *path);
static void *os_dlsym(File *handle, char *name);
static char *os_dlerror(void);

static bool os_system(char *command);

// Set maximum wait time between os_main calls
static void os_set_update_time(OS *os, u64 wake_time) {
    if (os->sleep_time > wake_time) {
        os->sleep_time = wake_time;
    }
}
