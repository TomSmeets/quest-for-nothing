// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// hot.c - Dynamically reload interactive programs
#include "inc.h"
#include "os_generic.h"
#include "fmt.h"

// Function types
typedef void *(main_init_t)(int argc, char *argv[]);
typedef void (main_update_t)(void *handle);


static void *load_module(char *path, u64 mtime) {
    mem m = {};

    // We have to copy the .so file to a unique destination
    // The linux dynamic library loader will silently return a cached copy otherwise.
    #if OS_WINDOWS
    // TODO: better location
    // TODO: FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE
    char *new_path = fmt(&m, "out/tmp-main-%u.dll", mtime);
    #else
    char *new_path = fmt(&m, "/tmp/tmp-main-%u.so", mtime);
    #endif
    os_printf("reloading %s\n", new_path);
    os_copy_file(path, new_path);

    // Don't unload library
    void *module = os_dlopen(new_path);
    assert(module);
    mem_clear(&m);
    return module;
}

void *main_init(int argc, char *argv[]) {
    // help text
    if(argc < 2 || str_eq(argv[1], "-h") || str_eq(argv[1], "--help")) {
        os_printf("Usage: %s FILE\n", argv[0]);
        os_printf("Dynamically start and keep reloading a program. The program at FILE has to be a dynamic library exposing the following 'main_init' and 'main_update' methods.\n");
        os_printf("\n");
        os_printf("The init method is called only once, start the app and return a handle to persistent data\n");
        os_printf("  void *main_init(int argc, char *argv[]);\n");
        os_printf("\n");
        os_printf("The update method is called continuously in a loop. Exit with os_exit(code)\n");
        os_printf("  void main_update(void *handle)\n");
        os_exit(1);
        return 0;
    }

    // Parse arguments
    char *module_path  = argv[1];
    char *trigger_path = "out/trigger";

    // Load application
    u64 prev_mtime = os_file_mtime(trigger_path);

    void *module = load_module(module_path, prev_mtime);
    main_init_t   *main_init   = os_dlsym(module, "main_init");
    main_update_t *main_update = os_dlsym(module, "main_update");

    void *handle = main_init(argc - 1, argv + 1);

    for(;;) {
        // poll mtime of the trigger
        u64 mtime = os_file_mtime(trigger_path);

        // if the file is newer that what we have seen before, perform a reload
        // also executed the very first time
        if(mtime > prev_mtime) {
            module = load_module(module_path, mtime);
            main_update = os_dlsym(module, "main_update");
            prev_mtime = mtime;
        }

        assert(main_update);
        main_update(handle);
    }
}

void main_update(void *handle) { os_exit(0); }
