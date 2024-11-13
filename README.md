# Quest For Nothing

The first version of this game. I will first implement v1.0, which will have very limited features. This will prevent me from getting stuck.

![](screenshot.png)

- os_api.h, types.h, input_type.h

# Version 1.0

- [ ] Player
  - [x] First person movement
  - [ ] Can die
  - [ ] Hitscan pistol

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

- Bugs
  - [ ] Textures have small border (blending artifact)

This should be enough for a first publishable version.

# Version 2.0

- [ ] Android
- [ ] Publish Play Store

# Building on Linux

First install the `clang` and `sdl2` packages.

## Standalone Executable

```bash
clang -O2 -g -o ./out/quest-for-nothing src/main.c
./out/quest-for-nothing
```

## Automatic Hot Reloading

```bash
# Compiles 'hot' executable
clang -O2 -g -o ./out/hot src/hot.c

# Compiles the game and runs it
# The game is updated when any source file in "src" is changed
./out/hot ./src/main.c

```

To run with the debugger use the following. If the source view in gdb is outdated run `dir` to reload it.

```bash
gdb --args ./out/hot ./src/main.c
```

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

## Everything at once
To build everything just run `./build.sh`

# Building on Windows

1. Install [Clang](https://github.com/llvm/llvm-project/releases)
2. Download and copy [SDL2.dll](https://github.com/libsdl-org/SDL/releases/) to the "out" directory.
3. Run the following commands

```bash
clang -O2 -g -o ./out/quest-for-nothing src/main.c
./out/quest-for-nothing.exe
```

To compile to WASM run the following commands
