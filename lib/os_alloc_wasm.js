ctx.exports.wasm_fail = (data, len) => {
    var bytes = new Uint8Array(ctx.memory.buffer, data, len)
    var string = new TextDecoder('utf8').decode(bytes)
    console.log(string)
    alert(string)
}
