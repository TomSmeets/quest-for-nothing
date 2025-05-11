// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fmt_vec.h - Formatter for vector types
#pragma once
#include "fmt.h"
#include "vec.h"

static void fmt_v3i(Fmt *fmt, v3i value) {
    fmt_siii(fmt, "v3i(", value.x, ", ", value.y, ", ", value.z, ")");
}

static void fmt_v3(Fmt *fmt, v3 value) {
    fmt_sfff(fmt, "v3(", value.x, ", ", value.y, ", ", value.z, ")");
}

static void fmt_sv3(Fmt *fmt, char *a0, v3 a1, char *a2) {
    fmt_s(fmt, a0);
    fmt_v3(fmt, a1);
    fmt_s(fmt, a2);
}
