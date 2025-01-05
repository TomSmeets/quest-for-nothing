// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// embed.h - Generated C code that statically embeds assets into the executable
#pragma once
#include "fmt.h"
#include "mem.h"
#include "os.h"
#include "rand.h"
#include "types.h"

// Just waiting for #embed to land in clang 19...
static void embed_file(Fmt *output, char *name, char *file_path) {
    File *input_file = os_open(file_path, Open_Read);

    fmt_ss(output, "static unsigned char ", name, "[] = {");
    for (;;) {
        u8 data[1024];
        u32 len = os_read(input_file, data, sizeof(data));
        if (len == 0) break;
        for (u32 i = 0; i < len; ++i) {
            if (i % 32 == 0) fmt_s(output, "\n    ");
            fmt_u(output, data[i]);
            fmt_s(output, ",");
        }
    }
    // Always include a terminating zero byte for strings
    fmt_s(output, "0,");
    fmt_s(output, "\n};\n");

    os_close(input_file);
}

// Embed all assets
static void embed_all_assets(Memory *tmp) {
    fmt_s(OS_FMT, "Generating asset.h\n");
    Fmt *asset_file = fmt_open(tmp, "out/asset.h");
    fmt_s(asset_file, "#pragma once\n");
    fmt_s(asset_file, "// clang-format off\n");
    embed_file(asset_file, "ASSET_SHADER_VERT", "src/gl_shader.vert");
    embed_file(asset_file, "ASSET_SHADER_FRAG", "src/gl_shader.frag");
    fmt_close(asset_file);
}
