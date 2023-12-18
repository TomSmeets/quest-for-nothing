// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// hot.c - Dynamically reload interactive programs
#include "inc.h"
#include "os_linux.h"
#include "fmt.h"

// Function types
typedef void *(Main_Init)(int argc, char *argv[]);
typedef u32   (Main_Update)(void *handle);

int main(int argc, char *argv[]) {
    mem m = {};

    // help text
    if(argc < 2 || str_eq(argv[1], "-h") || str_eq(argv[1], "--help")) {
        os_printf("Usage: %s FILE\n", argv[0]);
        os_printf("Dynamically start and keep reloading a program. The program at FILE has to be a dynamic library exposing the following 'main_init' and 'main_update' methods.\n");
        os_printf("\n");
        os_printf("The init method is called only once, start the app and return a handle to persistent data\n");
        os_printf("  void *main_init(int argc, char *argv[]);\n");
        os_printf("\n");
        os_printf("The update method is called continuously in a loop. Returning '0' means continue. Returning 'n' means exit with code 'n-1'\n");
        os_printf("  u32 main_update(void *handle)\n");
        return 1;
    }

    char *module_path  = argv[1];
    char *trigger_path = "out/trigger";

    // whatever main_init returned, all state should be stored here
    u64 prev_mtime = 0;

    // methods we load
    Main_Update *main_update = 0;
    void *handle = 0;
    for(;;) {
        // poll mtime of the trigger
        u64 mtime = os_file_mtime(trigger_path);

        // if the file is newer that what we have seen before, perform a reload
        // also executed the very first time
        if(mtime > prev_mtime || !main_update) {
            // We have to copy the .so file to a unique destination
            // The linux dynamic library loader will silently return a cached copy otherwise.
            char *new_path = fmt(&m, "/tmp/main-%u.so", mtime);
            os_printf("reloading %s\n", new_path);
            os_copy_file(module_path, new_path);

            // Don't unload library
            void *module = os_dlopen(new_path);
            assert(module);

            // first time
            if(!main_update) {
                Main_Init *main_init = os_dlsym(module, "main_init");
                assert(main_init);
                handle = main_init(argc - 1, argv + 1);
            }

            // get new address to main_update
            main_update = os_dlsym(module, "main_update");
            assert(main_update);
            mem_clear(&m);
            prev_mtime = mtime;
        }

        u32 ret = main_update(handle);
        if(ret > 0) return ret - 1;
    }
}
