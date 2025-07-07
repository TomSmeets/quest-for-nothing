// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// mutex.h
#pragma once
#include "types.h"

// References:
// - [Atomic break down: understanding ordering - Ciara](https://www.youtube.com/watch?v=C5xY96i0Aes)
// - https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
typedef struct {
    volatile u64 lock;
} Mutex;

static void mutex_lock(Mutex *mutex) {
    while (__atomic_exchange_n(&mutex->lock, 1, __ATOMIC_ACQUIRE)) {
#if !OS_IS_WASM
        __asm__ __volatile__("pause");
#endif
    }
}

static void mutex_unlock(Mutex *mutex) {
    __atomic_store_n(&mutex->lock, 0, __ATOMIC_RELEASE);
}
