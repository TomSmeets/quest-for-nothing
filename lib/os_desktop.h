#pragma once
#include "os_base.h"

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

#if OS_IS_LINUX
static File *os_open(char *path, OS_Open_Type type) {
    i32 fd = -1;
    if (type == Open_Write) {
        fd = linux_open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if (type == Open_Read) {
        fd = linux_open(path, O_RDONLY, 0);
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

static void os_sleep(u64 us) {
    struct linux_timespec time = linux_us_to_time(us);
    linux_nanosleep(&time, 0);
}

static File *os_dlopen(char *path) {
    return dlopen(path, RTLD_LOCAL | RTLD_NOW);
}

static void *os_dlsym(File *handle, char *name) {
    return dlsym(handle, name);
}

static char *os_dlerror(void) {
    return dlerror();
}

static bool os_system(char *cmd) {
    i32 ret = system(cmd);
    return ret >= 0;
}

#elif OS_IS_WINDOWS
static File *os_open(char *path, OS_Open_Type type) {
    HANDLE handle = 0;
    if (type == Open_Write) {
        handle = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    } else if (type == Open_Read) {
        handle = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

static File *os_dlopen(char *path) {
    return (File *)LoadLibrary(path);
}

static void *os_dlsym(File *file, char *name) {
    return GetProcAddress((void *)file, name);
}

static char *os_dlerror(void) {
    return 0;
}

static bool os_system(char *cmd) {
    int ret = system(cmd);
    return ret == 0;
}

#else
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
#endif
