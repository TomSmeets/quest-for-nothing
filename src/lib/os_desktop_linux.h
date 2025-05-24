// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_desktop_linux.h - Desktop functionality for Linux
#pragma once
#include "mem.h"
#include "os_base.h"
#include "os_desktop_types.h"
#include "str_mem.h"

static File *os_open(String path, OS_Open_Type type) {
    i32 fd = -1;
    if (type == Open_Write) {
        fd = linux_open(str_c(path), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if (type == Open_Read) {
        fd = linux_open(str_c(path), O_RDONLY, 0);
    }

    // Failed to open file
    if (fd <= 0) return 0;

    return fd_to_file(fd);
}

static void os_close(File *file) {
    assert(file, "File should be valid");
    i32 ret = linux_close(fd_from_file(file));
    assert(ret == 0, "Failed to close file");
}

static u32 os_read(File *file, u8 *data, u32 len) {
    assert(file, "Input file should be valid");
    i64 result = linux_read(fd_from_file(file), data, len);
    assert(result >= 0, "Failed to read");
    return result;
}

// Simple read/write
static String os_readfile(Memory *mem, String path) {
    i32 fd = linux_open(str_c(path), O_RDONLY, 0);
    if(fd < 0) return S0;

    struct linux_stat sb = {};
    linux_fstat(fd, &sb);

    // Does not fit memory
    if(sb.st_size > U32_MAX) {
        linux_close(fd);
        return S0;
    }

    u32 file_size = sb.st_size;
    u8 *file_buf = mem_push_uninit(mem, file_size);
    assert0(file_buf);

    u32 bytes_read = 0;
    while (bytes_read < file_size) {
        // Should be able to read everything at once
        i64 result = linux_read(fd, file_buf + bytes_read, file_size - bytes_read);

        // Something went wrong
        if (result <= 0) {
            linux_close(fd);
            return S0;
        }

        bytes_read += result;
    }

    linux_close(fd);

    return (String) { file_size, file_buf };
}

static void os_sleep(u64 us) {
    struct linux_timespec time = linux_us_to_time(us);
    linux_nanosleep(&time, 0);
}

static File *os_dlopen(String path) {
    return dlopen(str_c(path), RTLD_LOCAL | RTLD_NOW);
}

static void *os_dlsym(File *handle, String name) {
    return dlsym(handle, str_c(name));
}

static char *os_dlerror(void) {
    return dlerror();
}

static bool os_system(String cmd) {
    i32 ret = system(str_c(cmd));
    return ret == 0;
}
