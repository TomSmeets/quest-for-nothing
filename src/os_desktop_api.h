// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_desktop_api.h - Platofrm API for desktop operationg systems
#pragma once
#include "os_api.h"

typedef enum {
    Open_Write,
    Open_Read,
} OS_Open_Type;

static File *os_open(char *path, OS_Open_Type type);
static void os_close(File *file);

static u32 os_read(File *file, u8 *data, u32 len);
static void os_sleep(u64 time);

static File *os_dlopen(char *path);
static void *os_dlsym(File *handle, char *name);
