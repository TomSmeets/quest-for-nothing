// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// clang.h - Wrapper for invoking clang
#pragma once
#include "lib/fmt.h"
#include "lib/mem.h"

// Target platform
typedef enum {
    Platform_Linux,
    Platform_Windows,
    Platform_Wasm,
} Platform;

typedef struct {
    char *output_path;
    char *input_path;
    Platform platform;
    bool release;
    bool dynamic;
} Clang_Options;

static char *platform_to_string(Platform p) {
    if (p == Platform_Linux) return "Linux";
    if (p == Platform_Windows) return "Windows";
    if (p == Platform_Wasm) return "Wasm";
    return 0;
}

static void clang_fmt(Fmt *fmt, Clang_Options opt) {
    fmt_s(fmt, "clang");

    // Configure warning flags
    fmt_s(fmt, " -Wall -Werror");
    fmt_s(fmt, " -Wno-unused-function");
    fmt_s(fmt, " -Wno-unused-variable");
    fmt_s(fmt, " -Wno-unused-but-set-variable");
    fmt_s(fmt, " -Wno-format");

    // Inlcude paths
    fmt_s(fmt, " -Isrc");
    fmt_s(fmt, " --embed-dir=src");

    // Language flags
    fmt_s(fmt, " -std=c23");

    // We are running on this cpu
    // https://pkgstats.archlinux.de/compare/system-architectures/x86_64
    if (opt.platform != Platform_Wasm) {
        if (opt.release) {
            fmt_s(fmt, " -march=x86-64-v3");
        } else {
            fmt_s(fmt, " -march=native");
        }
    }

    // Platform flags
    if (opt.platform == Platform_Windows) {
#if OS_IS_WINDOWS
        // Compiling to windows from windows
        fmt_s(fmt, " -target x86_64-unknown-windows-msvc");
#else
        // Compiling to windows from linux
        fmt_s(fmt, " -target x86_64-unknown-windows-gnu");
#endif
    }

    // Webassembly
    if (opt.platform == Platform_Wasm) {
        fmt_s(fmt, " -target wasm32");
        fmt_s(fmt, " --no-standard-libraries");
        fmt_s(fmt, " -Wl,--no-entry");
        fmt_s(fmt, " -Wl,--export-all");
        fmt_s(fmt, " -fno-builtin");
        fmt_s(fmt, " -msimd128");
    }

    // Don't optimize, quick compile times
    if (opt.release) {
        fmt_s(fmt, " -O3");
        if (OS_IS_LINUX) fmt_s(fmt, " -Xlinker --strip-all");
    } else {
        fmt_s(fmt, " -O1 -g");
    }

    // Create a '.so' file for dynamic loading
    if (opt.dynamic) fmt_s(fmt, " -shared");

    // TODO: fmt_shell
    fmt_s(fmt, " -o ");
    fmt_s(fmt, opt.output_path);

    // TODO: fmt_shell
    fmt_s(fmt, " ");
    fmt_s(fmt, opt.input_path);
}

// Build single executable using clang
// This will become a single 'clang' call
static bool clang_compile(Memory *tmp, Clang_Options opt) {
    fmt_sss(G->fmt, "Compiling ", opt.input_path, " to ", opt.output_path, " in");
    if (opt.dynamic) fmt_s(G->fmt, " Dynamic");
    fmt_s(G->fmt, opt.release ? " Release" : " Debug");
    fmt_s(G->fmt, " mode for ");
    fmt_s(G->fmt, platform_to_string(opt.platform));
    fmt_s(G->fmt, "\n");

    if (opt.platform == Platform_Linux && OS_IS_WINDOWS) {
        fmt_s(G->fmt, "Cannot cross-compile to linux from windows.\n");
        return true;
    }

    Fmt *fmt = fmt_memory(tmp);
    clang_fmt(fmt, opt);
    char *cmd = fmt_close(fmt);
    return os_system(str_from(cmd));
}
