// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// watch.h - Simple linux inotify wrapper
#pragma once
#include "fmt.h"
#include "types.h"

// Include implementation code becase we depend on platform spesific code
#include "os_impl.h"

#if OS_IS_LINUX
typedef struct {
    i32 fd;
} Watch;

static void watch_init(Watch *watch, char *path) {
    i32 fd = linux_inotify_init(0);
    assert(fd >= 0, "Could not init inotify");
    watch->fd = fd;

    i32 wd = linux_inotify_add_watch(watch->fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);
    assert(wd >= 0, "inotify_add_watch");
}

static bool watch_changed(Watch *watch) {
    u32 change_count = 0;

    struct linux_timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    for (;;) {
        linux_fd_set fds = {};

        // Add fd to set
        fds.bits[watch->fd / 64] |= 1ull << (watch->fd % 64);

        int ret = linux_select(watch->fd + 1, &fds, 0, 0, &timeout);
        assert(ret >= 0, "select");

        if (ret == 0) {
            return change_count > 0;
        }

        u8 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

        i64 length = linux_read(watch->fd, buffer, sizeof(buffer));
        if (length < 0) {
            fmt_s(OS_FMT, "Failed to read data from watch\n");
            break;
        }

        struct inotify_event *event = (struct inotify_event *)buffer;
        if (event->len) {
            fmt_ss(OS_FMT, "changed: ", event->name, "\n");

            if (str_eq(event->name, "asset.h")) {
                continue;
            }
        }

        // Debounce (100 ms)
        timeout.tv_usec = 200 * 1000;
        change_count++;
    }

    return false;
}
#elif OS_IS_WINDOWS
typedef struct {
    HANDLE handle;
} Watch;

static void watch_init(Watch *watch, char *path) {
    HANDLE handle = FindFirstChangeNotification(path, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE);
    assert(handle != INVALID_HANDLE_VALUE, "Could not init FindFirstChangeNotification");
    watch->handle = handle;
}

static bool watch_changed(Watch *watch) {
    DWORD wait_status = WaitForSingleObject(watch->handle, 0);

    if (wait_status != WAIT_OBJECT_0) return false;

    for (;;) {
        FindNextChangeNotification(watch->handle);
        DWORD wait_status = WaitForSingleObject(watch->handle, 100);
        if (wait_status != WAIT_OBJECT_0) break;
    }

    return true;
}
#endif
