// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_alloc_wasm.js - Minimal OS functinality implemented for WASM

ctx.exports.wasm_fail = (data, len) => {
    var bytes = new Uint8Array(ctx.memory.buffer, data, len)
    var string = new TextDecoder('utf8').decode(bytes)
    console.log(string)
    alert(string)
}
