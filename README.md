# Quest For Nothing

![](screenshot.png)


# Building

Install `clang` and `sdl2`, then run the build script.

```bash
./build.sh linux
```

See `./build.sh` for more options:

```bash
Usage: ./build.sh <action> [args]...

Actions:
  run <main> [args]... Build and run with hot reloading
  watch                Build all targets and rebuild on every change
  all                  Build all targets
  linux                Build for linux
  windows              Build for windows
  web                  Build for web
  serve                Start a simple local python http server for testing the web version
  release              Build all targets in release mode
  asset                Build asset.h
  format               Format code

Examples:
  ./build.sh run src/main.c
  ./build.sh run src/hot.c watch
  ./build.sh build
  ./build.sh release
  ./build.sh asset
```

# Building windows
For cross compiling to windows you need `mingw-w64`.

```bash
./build.sh windows
```

# Running web version
For cross compiling to windows you need `wasm-ld` which is in the `lld` package on Arch Linux.

```bash
./build.sh web
```

To run the web version start a simple python http server after building. Then navigate to `http://localhost:8000/out`.

```bash
./build.sh serve
```

# Building Manually

Any executable can be compiled with a single 'clang' call.

Each executable is compiled as a single unit, with the platform automatically detected based on the `-target` passed to Clang.

- `clang -o ./out/main src/main.c`
- `clang -o ./out/build src/build.c`

Note that you will need to generate 'asset.h' once using `out/build asset` when compiling the game.

# Hot Reloading

Run `./out/build run src/main.c` to launch the game. Edit any file, and the game will reload while preserving its state.

For GDB, launch with `gdb --args ./out/build run ./src/main.c` and use the `dir` command to update the source view when needed.

# Short Term Taks
- Transition to "Entity" 
  - Make Player an Entity
  - Player and monster are the same, except for AI logic and Input
- Create overdraw shader: https://discussions.unity.com/t/how-to-build-or-where-to-get-the-overdraw-shader-used-in-the-scene-view/433807/9
- Work on sound system, make it like sfxr

# Version 1.0
I'd like to release this someday, so keeping it very simple!

- [ ] Player
  - [x] First-person movement
  - [ ] Can die
  - [x] Hit-scan pistol
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
  - [x] Simple aligned walls
  - [x] 2D layout
  - [ ] Collision
  - [x] Randomly generated
      1. [x] Create outline
      2. [x] Fill with maze
      3. [ ] Remove some walls with only one connection
      4. [ ] Decorate with textures

    - Generated
      - [ ] Genrate a few random cells (outline)
        - 0-3: place+move up,down,left,rigth
      - [ ] mirror on other side
      - [ ] Generate maze inside
      - [ ] Reduce wall count

- [ ] Sound
  - [ ] Simple synthesized music
  - [x] Simple synthesized sounds

- [x] Hot reloadable during development (`src/hot.c`)
- [x] Custom printf/sprintf (`src/fmt.h`)

- [ ] Publish
  - [ ] Shorter name?
  - [ ] Description
  - [ ] Video
  - [ ] Itch.io

- [x] Platforms
  - [x] Linux
  - [x] Windows
  - [x] Wasm

# Version 2.0 (ideas)

- [ ] Android
- [ ] Publish on Play Store
- [ ] Time travel effects?
