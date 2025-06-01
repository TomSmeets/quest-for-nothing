// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_desktop_none.h - Allow compilation for non-desktop os, all methods fail
#pragma once
#include "lib/os_base.h"
#include "lib/os_desktop_types.h"

static File *os_open(String path, OS_Open_Type type) {
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
static File *os_dlopen(String path) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void *os_dlsym(File *handle, String name) {
    os_fail("Platform is not a desktop");
    return 0;
}
#
