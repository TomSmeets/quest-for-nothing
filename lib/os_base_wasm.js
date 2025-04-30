// Time in micro seconds
ctx.exports.js_time = () => {
    return BigInt(new Date().getTime()*1000)
}

// Write utf8 text to the console
ctx.exports.js_write = (data, len) => {
    // data is a pointer in wasm memory
    var bytes = new Uint8Array(ctx.memory.buffer, data, len)

    // Convert utf8 bytes array to a js string
    var string = new TextDecoder('utf8').decode(bytes)

    // Log to the console.
    console.log(string)
}
