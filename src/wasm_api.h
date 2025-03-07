#pragma once
#include "types.h"

#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(#name)))

WASM_IMPORT(js_time)  u64  js_time(void);
WASM_IMPORT(js_write) void js_write(u8 *data, u32 len);

#define WASM_PAGE_SIZE 65536

static i32 wasm_memory_grow(i32 pages) {
    return __builtin_wasm_memory_grow(0, pages);
}
