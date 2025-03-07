#pragma once
#include "types.h"

#if __unix__
#define OS_IS_LINUX 1
#else
#define OS_IS_LINUX 0
#endif

#if _WIN32
#define OS_IS_WINDOWS 1
#else
#define OS_IS_WINDOWS 0
#endif

#if __wasm__
#define OS_IS_WASM 1
#else
#define OS_IS_WASM 0
#endif


#if OS_IS_LINUX
#include "linux_api.h"
#elif OS_IS_WINDOWS
#include <windows.h>
#elif OS_IS_WASM
#include "wasm_api.h"
#endif
