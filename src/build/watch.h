// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// watch.h - Simple linux inotify wrapper
#pragma once
#include "fmt.h"
#include "fs.h"
#include "os_api.h"
#include "os_main.h"
#include "types.h"

typedef struct Watch Watch;

// Add file or directory to watcher
static bool watch_add(Watch *watch, char *path);

// Check for changes
static bool watch_check(Watch *watch);

#if OS_IS_LINUX
struct Watch {
    u32 count;
    i32 fd;
};

static bool watch_add(Watch *watch, char *path) {
    fmt_ss(G->fmt, "Watch: ", path, "\n");
    Memory *mem = mem_new();
    for (FS_Dir *dir = fs_list(mem, str_from(path)); dir; dir = dir->next) {
        if (!dir->is_dir) continue;
        String path2 = str_cat3(mem, str_from(path), S("/"), dir->name);
        watch_add(watch, (char *)path2.data);
    }
    mem_free(mem);

    if (watch->count == 0) {
        watch->fd = linux_inotify_init(O_NONBLOCK);
        assert(watch->fd >= 0, "Could not init inotify");
    }

    i32 wd = linux_inotify_add_watch(watch->fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);
    assert(wd >= 0, "Filed to add path to watch");
    watch->count++;
    return true;
}

static bool watch_check_single(Watch *watch) {
    for (;;) {
        u8 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
        i64 length = linux_read(watch->fd, buffer, sizeof(buffer));

        // No more data
        if (length == -EAGAIN) {
            return false;
        }

        // Some other error
        if (length < 0) {
            fmt_s(G->fmt, "[E] Failed to read data from watch\n");
            return false;
        }

        // Change!
        struct inotify_event *event = (struct inotify_event *)buffer;
        // fmt_ss(G->fmt, "[D] changed: ", event->name, "\n");
        return true;
    }
}

static bool watch_check(Watch *watch) {
    if (!watch_check_single(watch)) return false;

    // Something changed, debounce
    for (;;) {
        // Debounce time
        os_sleep(50 * 1000);

        // No more changes
        if (!watch_check_single(watch)) return true;

        // Something changed, clear buffer and debounce again
        while (watch_check_single(watch));
    }
}
#elif OS_IS_WINDOWS
struct Watch {
    u32 count;
    HANDLE handle[16];
};

static bool watch_add(Watch *watch, char *path) {
    HANDLE handle = FindFirstChangeNotification(path, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE);
    assert(handle != INVALID_HANDLE_VALUE, "Could not init FindFirstChangeNotification");
    assert(watch->count < array_count(watch->handle), "No more space for watch handles");
    watch->handle[watch->count++] = handle;
    return true;
}

static bool watch_check_timeout(Watch *watch, u32 timeout) {
    DWORD wait_result = WaitForMultipleObjects(watch->count, watch->handle, FALSE, timeout);
    return wait_result >= WAIT_OBJECT_0 && wait_result < WAIT_OBJECT_0 + watch->count;
}

static bool watch_check(Watch *watch) {
    if (!watch_check_timeout(watch, 0)) return false;
    while (watch_check_timeout(watch, 100));
    return true;
}
#endif
