# Quest For Nothing

![](screenshot.png)


# Building

Install `clang` and `sdl2`, then run the build script.

```bash
./build.sh linux
```

See `./build.sh` for more options:

```
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

# Version 1.0
I'd like to release this someday, so keeping it very simple!

- Transition to "Entity" 
- Make Player an Entity
- Player and monster are the same, except for AI logic and Input
- Make Wall an entity (?)
- Create overdraw shader: https://discussions.unity.com/t/how-to-build-or-where-to-get-the-overdraw-shader-used-in-the-scene-view/433807/9
- Work on sound system, make it like sfxr
- Create a UI system
- Aliens can attack
- Player can Die
- Player can shoot
- Wall Collision
- Level Generation
- Improved General Sound system
- Music
- Write a good description
- Create Gameplay Video
- Create Development Video
- First 1.0 release on Itch.IO

# Version 2.0 (ideas)
- [ ] Android
- [ ] Publish on Play Store
- [ ] Time travel effects?
