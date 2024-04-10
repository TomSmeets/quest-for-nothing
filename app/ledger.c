#include "os_generic.h"

void *main_init(int argc, char **argv) {
    if(argc < 2) {
        os_print("ledger [file]\n");
        os_exit(1);
    }

    mem *m = mem_new();
    char *file = argv[1];
    Buffer buf = os_read_file(m, file);
    os_print((char *) buf.ptr);
    mem_free(m);
    return 0;
}

void main_update(void *handle) { os_exit(0); }
