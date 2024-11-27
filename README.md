# Quest For Nothing

![](screenshot.png)

# Building

Requirements: `clang` `sdl2` and `mingw`.

Build everything:

```bash
./build.sh
```


  |  --- | --- |
|out/main.elf | Standalone Linux (SDL/OpenGL) |
|out/main.exe | Standalone Windows (SDL/OpenGL) |
|out/main.wasm out/index.html | Web build (WebGL2) |

Build.sh will build every variation possible. While developing use the hot reloading.

# Hot reloading

Run `out/hot src/main.c` and then edit any file. The game reload while preserving it's state.

```bash
./out/hot ./src/main.c
```

To run with the debugger use the following. If the source view in gdb is outdated run `dir` to reload it.

```bash
gdb --args ./out/hot ./src/main.c
```

# Building Manually

Each executable can also be compiled manually. Following are some examples.

```bash
clang -o ./out/main.elf src/main.c
clang -o ./out/hot src/hot.c
```

# Version 1.0

- [ ] Player
  - [x] First person movement
  - [ ] Can die
  - [ ] Hitscan pistol
  - [x] Draw pistol
  - [x] Draw cross

- [x] Aliens
  - [x] Movement
    - [x] Idle: stand still
    - [x] Stroll: move randomly to another cell
    - [x] Attack: move to the player

  - [x] Generated Texture
    - [x] Eyes
    - [x] Body
    - [x] Color
    - [x] Walking animation

- [ ] Level
  - [ ] Simple aligned walls
  - [ ] 2D layout
  - [ ] Collision
  - [ ] Randomly generated
      1. [ ] Create outline
      2. [ ] Fill with maze
      3. [ ] Remove some walls with only one connection
      4. [ ] Decorate with textures

- [ ] Sound
  - [ ] Simple synthesized music
  - [ ] Simple synthesized sounds

- [x] Hot reloadable during development (`src/hot.c`)
- [x] Custom printf/sprintf (`src/fmt.h`)

- [ ] Publish
  - [ ] Shorter name?
  - [ ] Description
  - [ ] Video
  - [ ] Itch IO

- [ ] Platforms
  - [x] Linux
  - [x] Windows
  - [ ] Wasm

This should be enough for a first publishable version.

# Version 2.0

- [ ] Android
- [ ] Publish Play Store
- [ ] Time travel effects?


## Cross compile for Windows

To cross compile to windows install `mingw-w64` and copy [SDL2.dll](https://github.com/libsdl-org/SDL/releases/) to the "out" directory.
Then add `-target x86_64-unknown-windows-gnu` to the clang command line.

```bash
clang -O2 -g -target x86_64-unknown-windows-gnu -o ./out/quest-for-nothing.exe src/main.c
wine ./out/quest-for-nothing.exe
```


## To Web Assembly

Install `ldd` which provides `wasm-ld`.

```bash
clang -O2 -g -target wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export-all -fno-builtin -o ./out/quest-for-nothing.wasm src/main.c
```

| Option | Purpose |
| ------ | ------- |
| `-target wasm32` | Target wasm 32bit |
| `-Wl,--no-entry` | Don't create a main function |
| `-Wl,--export-all` | |
| `-fno-builtin` |  |

# Building on Windows

1. Install [Clang](https://github.com/llvm/llvm-project/releases)
2. Download and copy [SDL2.dll](https://github.com/libsdl-org/SDL/releases/) to the "out" directory.
3. Run the following commands

```bash
clang -O2 -g -o ./out/quest-for-nothing src/main.c
./out/quest-for-nothing.exe
```

To compile to WASM run the following commands
