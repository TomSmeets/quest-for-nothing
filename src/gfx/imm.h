// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// imm.h - Immediate mode variables
#pragma once
#include "lib/mem.h"
#include "lib/os_alloc.h"
#include "lib/str.h"

typedef struct {
    u32 iteration;
    u32 index;
    u32 size;
    u8 *data;
    Memory *mem;
} Imm;

static Imm imm_new(Memory *mem, u32 size) {
    return (Imm){
        .index = 0,
        .size = size,
        .data = mem_push_zero(mem, size),
        .mem = mem,
    };
}

static void imm_begin(Imm *imm) {
    imm->index = 0;
    imm->iteration++;
}

// Is this the first iteration?
static u32 imm_first(Imm *imm) {
    return imm->iteration == 1;
}

static void *imm_push(Imm *imm, u32 align, u32 size) {
    if (size > sizeof(void *)) {
        void **ptr = imm_push(imm, alignof(void *), sizeof(void *));
        if (!*ptr) *ptr = mem_push_zero(imm->mem, size);
        return *ptr;
    }

    u32 offset = std_align_offset(imm->data + imm->index, align);
    assert0(imm->index + offset + size <= imm->size);
    imm->index += offset;
    void *ret = imm->data + imm->index;
    imm->index += size;
    return ret;
}

#define imm_array(imm, type, count) ((type *)imm_push((imm), alignof(type), sizeof(type) * (count)))
#define imm_struct(imm, type) imm_array(imm, type, 1)
