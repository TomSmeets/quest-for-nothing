<!-- Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl> -->
<!-- programming.md - Describing how and why I like to program in C -->
# Programming in C
I like the C programming language due to it's simplicity, compilation speed, and runtime performance.

## Compilation Speed

Compilation speed is very important to me.
Compiling is something I do very often and it tells me whether the code is correct.
I compile my C code with `clang -O0`.

I include only that what I need from SDL2 ([src/sdl_api.h](src/sdl_api.h)) and from OpenGL ([src/gl_api.h](src/gl_api.h)).
This significantly increases the compilation speed.

I keep everything inside a single compilation unit.
This improves both compilation speed and eventual runtime performance of the program.
All my modules are header files with only a single `.c` file.
Just call `clang app/quest_for_nothing.c`, dependencies are included automatically.
I define all my methods as `static`, this prevents all these symbols from being exported, improving compilation speed and giving the compiler the chance to inline whatever it wants.
Link time optimization won't be as good as letting the compiler optimize everything at once.

See [build.sh](build.sh) for a implementation of this build process.

## Hot reloading
This is the best feature of C, which is not really possible in other languages.
Hot reloading in C is very easy because C gives you full control over the process memory.
See [app/hot.c](app/hot.c) and [app/hello.c](app/hello.c) for a complete implementation of live hot reloading.

## Memory management
I don't like malloc/free because I don't want to track the lifetime of individual objects.
It is also very slow and leads to fragmented and leaked memory.

I allocate everything with stack allocators.
They are very simple to implement and have good performance.
See [src/mem.h](src/mem.h) for the implementation.

```c
mem m = {};

// mem_push allocates uninitialized raw bytes
void *p = mem_push(m, 123);

// mem_struct allocates zero initialized types
u32 *q = mem_struct(m, u32);
```

## Initialize everything to zero
C does not have constructors and I almost never need them if I use zero as the default value.

## Style
- I will write everything myself so that I understand everything.
- Code should be simple / straightforward
- Most gameplay memory can live in mem_frame. Make use of this.
- Never think "blocking" or "async" Think polling or frames, this is the most flexible and best solution.

## Some unknowns
- `char *` vs `const char *`? I never use const, so why would I use this with strings? Are they special?


# Questionable Ideas

## Context

Tired of passing too many arguments? Just pack everything into a big 'context' struct.
This can be done with a global or a local argument.

Global make optimization a little harder which is why I also considered `ctx` as a argument I pass everywhere.

```c
struct ctx {
    bool init; // Is this the first frame?

    u32 frame; // Frame number
    f32 dt;    // Time between frames
    mem_arena mem_perm;  // memory stored forever!

    // Temporary memory for the current frame
    mem_arena mem_frame;
    // memory stored for TWO frames, so you can double buffer
    mem_arena mem_prev_frame;

    // "Global" data stored for each module
    // can store cached data, debug data, and whatever
    ctx_module mods[64];
};

#define ctx_get(name) ((name *) ctx_get_size((void **) &CTX->mods[CTX_MOD_ ## name].data, sizeof(name)))

static void *ctx_get_size(void **addr, u64 size) {
    ctx *ctx = ctx_ctx();
    if(!*addr) *addr = mem_push(&ctx->mem_perm, size);
    return *addr;
}

// Maybe we use ctx as a global
static void frame_end_gfx_gl(void) {
    gfx_gl_data *self = ctx_get(gfx_gl_data);
    gl_ptrs *gl = gl_get();
    if(!gl) return;
    // stuff ...
}

// Or as a local argument
static void frame_end_gfx_gl(ctx *c) {
    gfx_gl_data *self = ctx_get(c, gfx_gl_data);
    gl_ptrs *gl = gl_get(c);
    if(!gl) return;
    // stuff ...
}
```

### Conclusion

I created the previous instance of this game using this method. While I like the simplicity.
It hampered flexibility. It made the code less composable.
