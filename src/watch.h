// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// watch.h - Simple linux inotify wrapper
#pragma once
#include "fmt.h"
#include "os.h"
#include "types.h"
#include "os_api.h"

typedef struct {
    File *handle;
} Watch;

static void watch_init(Watch *watch, char *path);
static bool watch_changed(Watch *watch);

#if OS_IS_LINUX
static void watch_init(Watch *watch, char *path) {
    i32 fd = linux_inotify_init(O_NONBLOCK);
    assert(fd >= 0, "Could not init inotify");
    watch->handle = fd_to_file(fd);

    i32 wd = linux_inotify_add_watch(fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);
    assert(wd >= 0, "inotify_add_watch");
}

static bool watch_changed_quick(Watch *watch) {
    int fd = fd_from_file(watch->handle);
    for (;;) {
        u8 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

        i64 length = linux_read(fd, buffer, sizeof(buffer));

        // No more data
        if (length == -EAGAIN) {
            return false;
        }

        // Some other error
        if (length < 0) {
            fmt_s(G->fmt, "Failed to read data from watch\n");
            return false;
        }

        // Change!
        struct inotify_event *event = (struct inotify_event *)buffer;
        fmt_ss(G->fmt, "changed: ", event->name, "\n");
        return true;
    }
}

static bool watch_changed(Watch *watch) {
    if (!watch_changed_quick(watch)) return false;

    // Something changed, debounce
    for (;;) {
        // Debounce time
        os_sleep(50 * 1000);

        // No more changes
        if (!watch_changed_quick(watch)) return true;

        // Something changed, clear buffer and debounce again
        while (watch_changed_quick(watch));
    }
}
#elif OS_IS_WINDOWS
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
