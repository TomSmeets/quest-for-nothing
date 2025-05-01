// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_api_wasm.js - Platform syscall headers for WASM

// Global state
var ctx = {
    // Wasm memory
    memory: null,

    // Symbols defined in c exported to js
    imports: {},

    // Symbols defined in js exported to c
    exports: {},
};
