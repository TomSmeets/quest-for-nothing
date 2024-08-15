// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// hot.c - Dynamically compile and reload interactive programs
//
// Usage: ./hot src/main.c [ARGS]...
#include "os_api.h"
#include <dlfcn.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

// Configuration
static const u32 debounce_time = 100;
static const char *watch_path[] = {".", "src"};

// Implementation
typedef void os_main_t(OS *os);

static void embed_file(FILE *output, const char *name, const char *file_path) {
    // Just waiting for #embed to land in clang...
    FILE *f = fopen(file_path, "rb");
    fprintf(output, "static unsigned char FILE_%s[] = {", name);
    for (;;) {
        int c = fgetc(f);
        if (c <= 0) break;
        fprintf(output, "%u,", c);
    }
    fprintf(output, "0x00};\n");
    fclose(f);
}

static os_main_t *build_and_load(const char *main_path, u32 counter) {
    FILE *asset_file = fopen("src/asset.h", "w");
    fprintf(asset_file, "#pragma once\n");
    embed_file(asset_file, "SHADER_VERT", "src/gl_shader.vert");
    embed_file(asset_file, "SHADER_FRAG", "src/gl_shader.frag");
    fclose(asset_file);

    char out_path[1024];
    sprintf(out_path, "/tmp/hot-result-%u.so", counter);

#define CC_WARN                                                                                    \
    "-Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format"
#define CC_DEBUG "-march=native -O0 -g"
#define CC_RELEASE "-march=native -O2 -g0"

    char command[1024];
    sprintf(command, "clang " CC_WARN " " CC_DEBUG " -shared -o %s %s", out_path, main_path);

    printf("Running: %s\n", command);
    int ret = system(command);
    if (ret < 0) {
        perror("system");
        _exit(1);
    }

    if (ret != 0) {
        printf("Compile error!\n");
        return 0;
    }

    void *handle = dlopen(out_path, RTLD_LOCAL | RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return 0;
    }

    os_main_t *fcn = dlsym(handle, "os_main_dynamic");
    if (!fcn) {
        fprintf(stderr, "dlsym: %s\n", dlerror());
        return 0;
    }

    return fcn;
}

static bool watch_changed(int fd) {
    u32 change_count = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        int ret = select(fd + 1, &fds, 0, 0, &timeout);
        if (ret < 0) {
            perror("select");
            _exit(1);
        }

        if (ret == 0) {
            return change_count > 0;
        }

        if (!FD_ISSET(fd, &fds)) {
            continue;
        }

        u8 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

        ssize_t length = read(fd, buffer, sizeof(buffer));
        if (length < 0) {
            perror("read");
            break;
        }

        struct inotify_event *event = (struct inotify_event *)buffer;
        if (event->len) {
            printf("changed: %s\n", event->name);

            if (strcmp(event->name, "asset.h") == 0) {
                continue;
            }
        }

        // Debounce
        timeout.tv_usec = debounce_time * 1000;
        change_count++;
    }

    return false;
}

static int watch_init(void) {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        _exit(1);
    }
    for (u32 i = 0; i < sizeof(watch_path) / sizeof(watch_path[0]); ++i) {
        int wd = inotify_add_watch(fd, watch_path[i], IN_MODIFY | IN_CREATE | IN_DELETE);
        if (wd < 0) {
            perror("inotify_add_watch");
            _exit(1);
        }
    }

    return fd;
}

// Watch current directory
// recompile on change
// run application
int main(int argc, const char **argv) {
    if (argc < 2) {
        puts("hot <MAIN_FILE> [ARGS]...");
        return 1;
    }

    const char *main_path = argv[1];

    // Shift arguments by one
    OS os = {};
    os.argc = argc - 1;
    os.argv = (char **)argv + 1;

    // We use inotify to detect changes to source files
    int fd = watch_init();
    u32 counter = 0;

    // Build and load the first version
    os_main_t *update = build_and_load(main_path, counter++);

    for (;;) {
        // If a source file changed, reload it
        if (watch_changed(fd)) {
            update = build_and_load(main_path, counter++);
            os.reloaded = 1;
        }

        if (update) {
            update(&os);
            os.reloaded = 0;
        }
    }
}
