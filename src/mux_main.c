// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// mux_main.c - Everything App: tmux + vifm + helix + shell
//
// Lets start with a vifm clone
#include "global.h"
#include "os.h"
#include "fs.h"
#include "mem.h"

typedef struct Screen {
    u32 sx;
    u32 sy;
    u32 *data;
} Screen;

static void screen_clear(Screen *screen) {
    u32 center = screen->sx/2;
    for (u32 y = 0; y < screen->sy; ++y) {
        for (u32 x = 0; x < screen->sx; ++x) {
            u32 c = ' ';
            if (y == 0 || x == 0 || y == screen->sy - 1 || x == screen->sx - 1 || x == center) c = '.';
            screen->data[y * screen->sx + x] = c;
        }
    }
}

static Screen *screen_new(Memory *mem, u32 sx, u32 sy) {
    Screen *screen = mem_struct(mem, Screen);
    screen->sx = sx;
    screen->sy = sy;
    screen->data = mem_array_uninit(mem, u32, sx * sy);
    screen_clear(screen);
    return screen;
}

static void screen_draw(Screen *screen) {
    for (u32 y = 0; y < screen->sy; ++y) {
        for (u32 x = 0; x < screen->sx; ++x) {
            fmt_c(G->fmt, screen->data[y * screen->sx + x]);
        }
        fmt_c(G->fmt, '\n');
    }
}

struct App {
    Memory *mem;
};

static App *app_init(void) {
    Memory *mem = mem_new();

    App *app = mem_struct(mem, App);
    app->mem = mem;
    return app;
}

static void os_main(void) {
    // Initialize App
    if (!G->app) G->app = app_init();

    // Update Loop
    App *app = G->app;

    Memory *tmp = mem_new();
    fmt_s(G->fmt, "Update!\n");

    Screen *scr = screen_new(tmp, 128, 32);
    screen_clear(scr);

    u32 y = 1;
    for(FS_Dir *dir = fs_list(tmp, "."); dir; dir = dir->next) {
        u32 x = 2;

        char *name = dir->name;
        for(;;) {
            if(*name == 0) break;
            if(x >= 32) break;
            scr->data[y*scr->sx + x] = *name;
            name++;
            x++;
        }

        if(dir->is_dir) {
            scr->data[y*scr->sx + x] = '/';
        }

        y++;
    }

    screen_draw(scr);

    G->os->sleep_time = 1*1000*1000;
    mem_free(tmp);
}
