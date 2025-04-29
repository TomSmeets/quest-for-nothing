// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os.h - Base platform API
#pragma once
#include "fmt.h"
#include "global.h"
#include "os_desktop.h"
#include "os_main_types.h"
#include "rand.h"
#include "types.h"

// Set maximum wait time between os_main calls
static void os_set_update_time(OS *os, u64 wake_time) {
    if (os->sleep_time > wake_time) {
        os->sleep_time = wake_time;
    }
}

#if OS_IS_LINUX
#include "os_main_linux.h"
#elif OS_IS_WINDOWS
#include "os_main_windows.h"
#elif OS_IS_WASM
#include "os_main_wasm.h"
#endif
