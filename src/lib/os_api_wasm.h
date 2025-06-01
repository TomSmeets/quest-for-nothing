// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_api_wasm.h - Platform syscall headers for WASM
#pragma once
#include "lib/types.h"
#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(#name)))
#define WASM_PAGE_SIZE 65536
