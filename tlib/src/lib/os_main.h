// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main.h - Main entry point
#pragma once
#include "lib/fmt.h"
#include "lib/global.h"
#include "lib/os_desktop.h"
#include "lib/os_main_types.h"
#include "lib/rand.h"
#include "lib/types.h"

// Set maximum wait time between os_main calls
static void os_set_update_time(OS *os, u64 wake_time) {
    if (os->sleep_time > wake_time) {
        os->sleep_time = wake_time;
    }
}

#if OS_IS_LINUX
#include "lib/os_main_linux.h"
#elif OS_IS_WINDOWS
#include "lib/os_main_windows.h"
#elif OS_IS_WASM
#include "lib/os_main_wasm.h"
#endif
