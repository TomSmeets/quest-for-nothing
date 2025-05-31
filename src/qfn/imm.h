// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// imm.h - Immediate mode variables
#pragma once
#include "os_alloc.h"
#include "str.h"

typedef struct {
    u32 index;
    u32 size;
    u8 *data;
} Imm;

static Imm imm_new(String buffer) {
    return (Imm){
        .index = 0,
        .size = buffer.len,
        .data = buffer.data,
    };
}

static void imm_start(Imm *imm) {
    imm->index = 0;
}

static void *imm_push(Imm *imm, u32 align, u32 size) {
    u32 offset = std_align_offset(imm->data + imm->index, align);
    assert0(imm->index + offset + size <= imm->size);

    imm->index += offset;
    void *ret = imm->data + imm->index;
    imm->index += size;
    return ret;
}

#define imm_array(imm, type, count) ((type *)imm_push((imm), alignof(type), sizeof(type) * (count)))
#define imm_struct(imm, type, count) imm_array(imm, type, 1)
