#pragma once
#include "os_api.h"
#include "types.h"

// On Linux, we can use the 'mmap' system-call.
// mmap can also map files to memory, which is not used in this case.
static void *os_alloc(u32 size) {
    void *ret = linux_mmap(
        // Let the system choose a starting address for us
        0,
        // Allocation size
        size,
        // Memory should be read and writable
        PROT_READ | PROT_WRITE,
        // Allocation is not backed by a file
        MAP_PRIVATE | MAP_ANONYMOUS,
        // File descriptor and offset are not used
        -1, 0
    );
    if (ret == MAP_FAILED) return 0;
    return ret;
}

static void os_fail(char *message) {
    linux_write(1, message, str_len(message));
    linux_exit_group(1);
}
