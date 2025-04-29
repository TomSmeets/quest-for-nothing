#include "os_alloc.h"
#include "std.h"
#include "str.h"
#include "types.h"
static u64 os_time(void) {
    LARGE_INTEGER big_freq, big_count;
    assert(QueryPerformanceFrequency(&big_freq), "Failed to get performance frequency");
    assert(QueryPerformanceCounter(&big_count), "Failed to get performance counter");
    i64 freq = big_freq.QuadPart;
    i64 count = big_count.QuadPart;
    assert(freq >= 1000 * 1000, "Invalid performance frequency");
    assert(count >= 0, "Invalid performance counter");
    return (u64)count / ((u64)freq / 1000 / 1000);
}

static u64 os_rand(void) {
    // A crappy rng, but fine for now
    return os_time();
}

static void os_write(File *file, u8 *data, u32 len) {
    WriteFile(file, data, len, 0, 0);
}

static void os_exit(i32 code) {
    ExitProcess(code);
}

static void windows_print(char *message) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(out, message, str_len(message), 0, 0);
}

static void *os_alloc_raw(u32 size) {
    void *alloc = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(alloc, "Failed to allocate memory");
    return alloc;
}

