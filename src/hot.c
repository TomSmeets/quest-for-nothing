// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// hot.c - Dynamically compile and reload interactive programs
//
// Usage: ./hot src/main.c [ARGS]...
#include "fmt.h"
#include "mem.h"
#include "os.h"
#include "os_api.h"
#include "rand.h"
#include "types.h"

// Configuration
static const u32 watch_debounce = 100;
static const char *watch_path[] = {".", "src"};
static const char *watch_embed[][2] = {
    {"FILE_SHADER_VERT", "src/gl_shader.vert"},
    {"FILE_SHADER_FRAG", "src/gl_shader.frag"},
};

static const char *compile_command = "clang"
                                     // Enable most warning flags
                                     " -Wall -Werror"
                                     " -Wno-unused-function"
                                     " -Wno-unused-variable"
                                     " -Wno-unused-but-set-variable"
                                     " -Wno-format"
                                     // We are running on this cpu
                                     " -march=native"
                                     // Don't optimize, quick compile times
                                     " -O0 -g"
                                     " -std=c23"
                                     // Create a '.so' file for dynamic loading
                                     " -shared"
                                     " -o %s %s";
// Implementation
typedef void os_main_t(OS *os);

static void embed_file(u32 output, char *name, char *file_path) {
    // Just waiting for #embed to land in clang...
    u32 fd = os_open(file_path, Open_Write);
    os_fprintf(output, "static unsigned char %s[] = {", name);
    for (;;) {
        u8 data[1024];
        ssize_t len = os_read(fd, data, sizeof(data));
        assert(len >= 0, "Failed to read data");
        if (len == 0) break;
        for (u32 i = 0; i < len; ++i) {
            os_fprintf(output, "%u,", data[i]);
        }
    }
    os_fprintf(output, "0};\n");
    os_close(fd);
}

static void embed_files(char *output_file) {
    u32 asset_file = os_open(output_file, Open_Write);
    assert(asset_file >= 0, "Failed to open asset output file");
    os_fprintf(asset_file, "#pragma once\n");
    os_fprintf(asset_file, "// clang-format off\n");
    for (u32 i = 0; i < array_count(watch_embed); ++i) {
        embed_file(asset_file, watch_embed[i][0], watch_embed[i][1]);
    }
    os_close(asset_file);
}

static os_main_t *build_and_load(char *main_path, u64 counter) {
    embed_files("src/asset.h");

    Memory *tmp = mem_new();

    char *out_path = fmt(tmp, "/tmp/hot-%08llx.so", counter);
    char *command = fmt(tmp, compile_command, out_path, main_path);

    os_printf("Running: %s\n", command);
    int ret = system(command);
    assert(ret >= 0, "Error while compiling");

    mem_free(tmp);

    if (ret != 0) {
        os_print("Compile error!\n");
        return 0;
    }

    void *handle = dlopen(out_path, RTLD_LOCAL | RTLD_NOW);
    if (!handle) {
        os_printf("dlopen: %s\n", dlerror());
        return 0;
    }

    os_main_t *fcn = dlsym(handle, "os_main_dynamic");
    if (!fcn) {
        os_printf("dlsym: %s\n", dlerror());
        return 0;
    }

    return fcn;
}

// === Watch ===
static int watch_init(void) {
    int fd = inotify_init();
    assert(fd >= 0, "Could not init inotify");
    for (u32 i = 0; i < array_count(watch_path); ++i) {
        int wd = inotify_add_watch(fd, watch_path[i], IN_MODIFY | IN_CREATE | IN_DELETE);
        assert(wd >= 0, "inotify_add_watch");
    }

    return fd;
}

static bool watch_changed(int fd) {
    u32 change_count = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    for (;;) {
        fd_set fds = {};
        FD_SET(fd, &fds);
        int ret = select(fd + 1, &fds, 0, 0, &timeout);
        assert(ret >= 0, "select");

        if (ret == 0) {
            return change_count > 0;
        }

        if (!FD_ISSET(fd, &fds)) {
            continue;
        }

        u8 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

        ssize_t length = read(fd, buffer, sizeof(buffer));
        if (length < 0) {
            os_print("Failed to read data from watch\n");
            break;
        }

        struct inotify_event *event = (struct inotify_event *)buffer;
        if (event->len) {
            os_printf("changed: %s\n", event->name);

            if (str_eq(event->name, "asset.h")) {
                continue;
            }
        }

        // Debounce
        timeout.tv_usec = watch_debounce * 1000;
        change_count++;
    }

    return false;
}

typedef struct {
    // For generating unique '.so' names
    Random rng;

    // main.c
    char *main_path;

    // OS struct for the child
    OS child_os;

    // File watch
    int watch;

    // Update function
    bool first_time;
    os_main_t *update;
} Hot;

static Hot *hot_load(OS *os) {
    // Already loaded
    if (os->app) return os->app;

    Memory *mem = mem_new();
    Hot *hot = mem_struct(mem, Hot);

    // Parse arguments
    if (os->argc < 2) {
        os_printf("%s <MAIN_FILE> [ARGS]...\n", os->argv[0]);
        os_exit(1);
    }

    hot->main_path = os->argv[1];

    // Prepare OS handle for the child
    hot->child_os.argc = os->argc - 1;
    hot->child_os.argv = os->argv + 1;

    // Init inotify
    hot->watch = watch_init();
    hot->first_time = 1;

    // Init rng, nested 'hot' would not work otherwise
    hot->rng.seed = os_rand();

    // Save to os
    os->app = hot;
    return hot;
}

void os_main(OS *os) {
    Hot *hot = hot_load(os);

    if (hot->first_time || watch_changed(hot->watch)) {
        hot->update = build_and_load(hot->main_path, rand_u32(&hot->rng));
        hot->child_os.reloaded = 1;
        hot->first_time = 0;
    }

    if (hot->update) {
        hot->update(&hot->child_os);
        os->sleep_time = hot->child_os.sleep_time;
        hot->child_os.reloaded = 0;
    } else {
        os->sleep_time = 100 * 1000;
    }
}
