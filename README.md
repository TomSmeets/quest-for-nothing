# Quest For Nothing

![](screenshot.png)


# Building

Install `clang`, `ldd`, `sdl2`, and `mingw-w64`, then run the build script.


# Building Manually

Any executable can be compiled with a single 'clang' call.

Each executable is compiled as a single unit, with the platform automatically detected based on the `-target` passed to Clang.

- `clang -o ./out/main src/main.c`
- `clang -o ./out/hot src/hot.c`

Note that you will need to generate 'asset.h' once using `out/hot asset` when compiling the game.

# Hot Reloading

Run `./out/hot run src/main.c` to launch the game. Edit any file, and the game will reload while preserving its state.

For GDB, launch with `gdb --args ./out/hot run ./src/main.c` and use the `dir` command to update the source view when needed.

# Version 1.0

I'd like to release this someday, so keeping it very simple!

- [ ] Player
  - [x] First-person movement
  - [ ] Can die
  - [ ] Hitscan pistol
  - [x] Draw pistol
  - [x] Draw cross

- [x] Aliens
  - [x] Movement
    - [x] Idle: stand still
    - [x] Stroll: move randomly to another cell
    - [x] Attack: move to the player

  - [x] Generated texture
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
  - [ ] Itch.io

- [ ] Platforms
  - [x] Linux
  - [x] Windows
  - [ ] Wasm

# Version 2.0 (ideas)

- [ ] Android
- [ ] Publish on Play Store
- [ ] Time travel effects?
