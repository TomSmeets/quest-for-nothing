// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_desktop_windows.h - Desktop functionality for Windows
#pragma once
#include "lib/os_base.h"
#include "lib/os_desktop_types.h"
#include "lib/str_mem.h"

static File *os_open(String path, OS_Open_Type type) {
    HANDLE handle = 0;
    if (type == Open_Write) {
        handle = CreateFile(str_c(path), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    } else if (type == Open_Read) {
        handle = CreateFile(str_c(path), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }

    if (handle == INVALID_HANDLE_VALUE) handle = 0;
    return handle;
}

static void os_close(File *file) {
    CloseHandle(file);
}

static u32 os_read(File *file, u8 *data, u32 len) {
    DWORD bytes_read = 0;
    BOOL success = ReadFile(file, data, len, &bytes_read, 0);
    return bytes_read;
}

static void os_sleep(u64 time) {
    Sleep(time / 1000);
}

static File *os_dlopen(String path) {
    return (File *)LoadLibrary(str_c(path));
}

static void *os_dlsym(File *file, String name) {
    return GetProcAddress((void *)file, str_c(name));
}

static char *os_dlerror(void) {
    return 0;
}

static bool os_system(String cmd) {
    int ret = system(str_c(cmd));
    return ret == 0;
}
