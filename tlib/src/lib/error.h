// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// result.h - Common error type
#pragma once

// 0 -> no error
// x -> some error
typedef enum {
    Error_None = 0,
    Error_EOF,
    Error_Data,
} Error;
