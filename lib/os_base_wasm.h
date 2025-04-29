#include "os_alloc.h"
#include "std.h"
#include "str.h"
#include "types.h"

WASM_IMPORT(js_time) u64 js_time(void);
WASM_IMPORT(js_write) void js_write(u8 *data, u32 len);

static u64 os_time(void) {
    return js_time();
}

static File *os_stdout(void) {
    return (File *)1;
}

static void os_write(File *file, u8 *data, u32 len) {
    js_write(data, len);
}

static void os_exit(i32 code) {
    __builtin_trap();
}

// we need to implement these if we don't use stdlib
// C wil very "helpfully" detect memcpy and memset for loops.
// And translate them to a call to memcpy and memset.
// TODO: Test if this is still the case
void *memcpy(void *restrict dst, const void *restrict src, u32 size) {
    std_memcpy(dst, src, size);
    return dst;
}

void *memset(void *restrict dst, u8 value, u32 size) {
    std_memset(dst, value, size);
    return dst;
}
