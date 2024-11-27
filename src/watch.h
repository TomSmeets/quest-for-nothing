// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// watch.h - Simple linux inotify wrapper
#pragma once
#include "fmt.h"
#include "os.h"
#include "os_api.h"
#include "types.h"

typedef struct {
    int fd;
} Watch;

#if OS_IS_LINUX

static void watch_init(Watch *watch) {
    int fd = inotify_init();
    assert(fd >= 0, "Could not init inotify");
    watch->fd = fd;
}

static void watch_add(Watch *watch, char *path) {
    int wd = inotify_add_watch(watch->fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);
    assert(wd >= 0, "inotify_add_watch");
}

static bool watch_changed(Watch *watch) {
    u32 change_count = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    for (;;) {
        fd_set fds = {};
        FD_SET(watch->fd, &fds);
        int ret = select(watch->fd + 1, &fds, 0, 0, &timeout);
        assert(ret >= 0, "select");

        if (ret == 0) {
            return change_count > 0;
        }

        if (!FD_ISSET(watch->fd, &fds)) {
            continue;
        }

        u8 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

        ssize_t length = read(watch->fd, buffer, sizeof(buffer));
        if (length < 0) {
            os_print("Failed to read data from watch\n");
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
        timeout.tv_usec = 100 * 1000;
        change_count++;
    }

    return false;
}
#else
static void watch_init(Watch *watch) {
}
static void watch_add(Watch *watch, char *path) {
}
static bool watch_changed(Watch *watch) {
    return false;
}
#endif
