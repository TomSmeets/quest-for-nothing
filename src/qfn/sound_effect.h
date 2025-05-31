// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_effect.h - Sound effects
#pragma once
#include "sound_var.h"
#include "vec.h"

// Clip volume in range -1 to 1
static f32 sound_clip(f32 sample) {
    if (sample > 1.0f) sample = 1.0f;
    if (sample < -1.0f) sample = -1.0f;
    return sample;
}

static v2 sound_clip2(v2 sample) {
    sample.x = sound_clip(sample.x);
    sample.y = sound_clip(sample.y);
    return sample;
}

// Clip volume in range -1 to 1 by folding inwards
static f32 sound_fold(f32 sample) {
    for (;;) {
        if (sample > 1.0f) {
            sample = 1.0f - sample;
            continue;
        }
        if (sample < -1.0f) {
            sample = -1.0f - sample;
            continue;
        }
        return sample;
    }
}

typedef struct {
    u32 next_index;
    f32 values[SOUND_SAMPLE_RATE];
} Delay_Buffer;

static void delay_write(Delay_Buffer *buf, f32 value) {
    buf->values[buf->next_index] = value;
    buf->next_index += 1;
    buf->next_index %= array_count(buf->values);
}

static f32 delay_read(Delay_Buffer *buf, f32 time) {
    if (time < 0) return 0.0f;
    u32 offset = time * SOUND_SAMPLE_RATE;
    if (offset >= array_count(buf->values)) return 0.0f;

    u32 index = buf->next_index;
    index += array_count(buf->values); // Ensure index stays positive
    index -= 1;                        // Go to current index
    index -= offset;                   // Apply offset (index should stay positive)
    index %= array_count(buf->values); // Wrap around if needed
    return buf->values[index];
}

static bool sound_changed(Sound *sound, u32 value) {
    u32 *prev = sound_var(sound, u32);
    bool ret = *prev != value;
    *prev = value;
    return ret || (sound->sample == 1);
}
