// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_base_wasm.js - Base OS implementation for WASM

// Time in micro seconds
ctx.exports.wasm_time = () => {
    return BigInt(new Date().getTime()*1000)
}

// Write utf8 text to the console
ctx.exports.wasm_write = (data, len) => {
    // data is a pointer in wasm memory
    var bytes = new Uint8Array(ctx.memory.buffer, data, len)

    // Convert utf8 bytes array to a js string
    var string = new TextDecoder('utf8').decode(bytes)

    // Log to the console.
    console.log(string)
}
