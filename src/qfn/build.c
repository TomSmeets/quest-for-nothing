// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "build/build.h"
#include "lib/os_main.h"

struct App {
    Build *build;
};

static bool fs_exists(String path) {
    File *f = os_open(path, Open_Read);
    if (!f) return false;
    os_close(f);
    return true;
}

static void cmd_release(App *app, Cli *cli) {
    bool active = cli_command(cli, "release", "Build qfn release");
    bool upload_itch = cli_flag(cli, "--upload-itch", "Upload to itch.io");
    bool upload_web = cli_flag(cli, "--upload-web", "Upload to tsmeets.nl");

    if (!active) return;
    os_system(S("rm -rf out/release/ && mkdir -p out/release/"));

    Clang_Options opts = {};
    opts.includes = app->build->sources;
    opts.input_path = "src/qfn/qfn.c";
    opts.release = true;

    opts.platform = Platform_Linux;
    opts.output_path = "out/release/quest_for_nothing.elf";
    if (!clang_compile(opts)) os_exit(1);

    opts.platform = Platform_Windows;
    opts.output_path = "out/release/quest_for_nothing.exe";
    if (!clang_compile(opts)) os_exit(1);

    opts.platform = Platform_Wasm;
    opts.output_path = "out/release/quest_for_nothing.wasm";
    if (!clang_compile(opts)) os_exit(1);

    // Download SDL3
    if (!fs_exists(S("out/SDL3.dll"))) {
        os_system(S("curl -L -o out/SDL3.zip https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-3.2.16-win32-x64.zip"));
        os_system(S("cd out/ && unzip SDL3.zip SDL3.dll"));
    }
    os_system(S("cp out/SDL3.dll out/release/SDL3.dll"));
    os_system(S("cp src/lib/*.js out/release/"));
    os_system(S("cp src/gfx/*.js out/release/"));
    os_system(S("cp src/qfn/index.html out/release/index.html"));

    if (upload_itch) {
        os_system(S("butler push out/release tsmeets/quest-for-nothing:release --userversion $(date +'%F')"));
        os_system(S("butler push out/release tsmeets/quest-for-nothing:release-web --userversion $(date +'%F')"));
    }
    if (upload_web) {
        os_system(S("rclone copy out/release fastmail:tsmeets.fastmail.com/files/tsmeets.nl/qfn/"));
    }
    os_exit(0);
}

static void os_main(void) {
    if (!G->app) {
        G->app = mem_struct(G->mem, App);
        G->app->build = build_new();
    }

    Cli *cli = cli_new();
    cmd_release(G->app, cli);
    build_update(G->app->build, cli);
}
