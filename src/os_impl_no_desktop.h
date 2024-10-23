#pragma once
#include "os_api.h"

static File *os_open(char *path, OS_Open_Type type) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void os_close(File *file) {
    os_fail("Platform is not a desktop");
}

static u32 os_read(File *file, u8 *data, u32 len) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void os_sleep(u64 time) {
    os_fail("Platform is not a desktop");
}

static File *os_dlopen(char *path) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void *os_dlsym(File *handle, char *name) {
    os_fail("Platform is not a desktop");
    return 0;
}
