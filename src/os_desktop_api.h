#pragma once
#include "types.h"

typedef enum {
    Open_Write,
    Open_Read,
} OS_Open_Type;

static u32 os_open(char *path, OS_Open_Type type);
static void os_close(u32 fd);
static u32 os_read(u32 fd, u8 *data, u32 len);
static void os_sleep(u64 time);

static void *os_dlopen(char *path);
static void *os_dlsym(void *handle, char *name);
