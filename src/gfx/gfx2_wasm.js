

ctx.exports.wasm_gfx_init = () => {
    // Scale canvas by some factor to achive MSAA
    const multisample_scaling = 2

    ctx.canvas = document.getElementById('canvas')

    // Input listeners
    document.addEventListener("keydown",   (ev) => { ctx.imports.wasm_gfx_key_down(ev.keyCode, true) }, true)
    document.addEventListener("keyup",     (ev) => { ctx.imports.wasm_gfx_key_down(ev.keyCode, false) }, true)
    document.addEventListener("mousemove", (ev) => { ctx.imports.wasm_gfx_mouse_move(ev.x, ev.y, ev.movementX, ev.movementY) }, true)
    document.addEventListener("mousedown", (ev) => { ctx.imports.wasm_gfx_mouse_down(ev.button, true) }, true)
    document.addEventListener("mouseup",   (ev) => { ctx.imports.wasm_gfx_mouse_down(ev.button, false) }, true)
    window.addEventListener("resize",      (ev) => {
        canvas.width = window.innerWidth * multisample_scaling;
        canvas.height = window.innerHeight * multisample_scaling;
        ctx.imports.wasm_gfx_resize(window.innerWidth, window.innerHeight);
    }, false);

    // Send initial size to wasm
    canvas.width = window.innerWidth*multisample_scaling;
    canvas.height = window.innerHeight*multisample_scaling;
    ctx.imports.gfx_js_resize(window.innerWidth, window.innerHeight);

    // Load Opengl
    const texture_size = 4096;
    var gl = canvas.getContext("webgl2", {
        alpha: false,
        depth: true,
        antialias: false,
        powerPreference: "high-performance",
        // desynchoronized: true,
    });
    ctx.gl = gl;
}

ctx.exports.wasm_gfx_quit = () => { }

ctx.exports.wasm_gfx_begin = () => { }
ctx.exports.wasm_gfx_end = () => { }

ctx.exports.wasm_gfx_set_grab = (grab) => {
    if(grab) {
        canvas.requestPointerLock()
    } else {
        document.exitPointerLock()
    }
}

ctx.exports.wasm_gfx_set_fullscreen = (full) => {
    if(full) {
        canvas.requestFullscreen()
    } else {
        document.exitFullscreen()
    }
}

