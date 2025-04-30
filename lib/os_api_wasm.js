// Global state
var ctx = {
    // Wasm memory
    memory: null,

    // Symbols defined in c exported to js
    imports: {},

    // Symbols defined in js exported to c
    exports: {},
};
