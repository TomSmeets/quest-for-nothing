#pragma once
#include "std.h"
#include "str.h"

#if __unix__
#define OS_IS_LINUX 1
#define OS_IS_WINDOWS 0
#define OS_IS_WASM 0
#include "linux_api.h"
#endif

#if _WIN32
#define OS_IS_LINUX 0
#define OS_IS_WINDOWS 1
#define OS_IS_WASM 0
#include <windows.h>
#endif

#if __wasm__
#define OS_IS_LINUX 0
#define OS_IS_WINDOWS 0
#define OS_IS_WASM 1

#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(#name)))
#define WASM_PAGE_SIZE 65536
#endif
