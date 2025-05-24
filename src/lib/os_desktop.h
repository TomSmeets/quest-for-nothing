// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_desktop.h - Desktop functionality: files, commands
#pragma once
#include "os_base.h"
#include "mem.h"
#include "os_desktop_types.h"

static File *os_open(String path, OS_Open_Type type);
static void os_close(File *file);
static u32 os_read(File *file, u8 *data, u32 len);

// Simple read/write
static String os_readfile(Memory *mem, String path);
static void os_writefile(String path, String data);

// os_write is defined in os_base

// Block thread for some number of microseconds
static void os_sleep(u64 time);

// Dynamicllay load library and applications
static File *os_dlopen(String path);
static void *os_dlsym(File *handle, String name);
static char *os_dlerror(void);

// Run system command
static bool os_system(String command);

#if OS_IS_LINUX
#include "os_desktop_linux.h"
#elif OS_IS_WINDOWS
#include "os_desktop_windows.h"
#else
#include "os_desktop_none.h"
#endif
