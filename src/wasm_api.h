#pragma once
#include "std.h"
#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(#name)))
