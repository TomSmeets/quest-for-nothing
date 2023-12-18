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
