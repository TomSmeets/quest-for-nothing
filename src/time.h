// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// time.h - Accurate frame timing calcuation
#pragma once
#include "fmt.h"
#include "os.h"
#include "types.h"

typedef struct {
    u64 frame_start;
    u64 frame_end;
    u64 compute_time;
} Time_Info;

static f32 time_begin(Time_Info *time, u64 target_rate) {
    u64 target_increment = 1e6 / target_rate;
    u64 increment = target_increment;
    while (time->compute_time > increment)
        increment += target_increment;

    // os_printf("target=%u us compute=%u us increment=%u us\n", target_increment, time->compute_time, increment);

    u64 frame_time = time->frame_end;

    u64 now = os_time();
    if (frame_time + increment < now) {
        frame_time = now;
        os_printf("Ahead\n");
    }

    time->frame_start = frame_time;
    time->frame_end = frame_time + increment;
    return (f32)increment / 1e6f;
}

static u64 time_end(Time_Info *time) {
    u64 now = os_time();

    u64 compute_time = now - time->frame_start;

    if (now < time->frame_start) compute_time = 0;

    if (compute_time > 500 * 1000) compute_time = 500 * 1000;

    time->compute_time = compute_time;
    if (time->frame_end < now) {
        // Missed a frame
        os_printf("missed a frame, compute=%.2f fps\n", 1e6f / (f32)compute_time);
        return 0;
    }

    u64 sleep_time = time->frame_end - now;
    return sleep_time;
}
