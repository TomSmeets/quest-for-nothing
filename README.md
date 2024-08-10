# Quest For Nothing

The first version of this game. I will first implement v1.0, which will have very limited features. This will prevent me from getting stuck.

## How to run

Install `clang` then run the following command.

```bash
./build.sh && ./out/hot ./src/main.c
```

## Version 1.0

- Player
  - First person movement
  - Can die
  - Hitscan pistol

- Aliens
  - Movement
    - Idle: stand still
    - Stroll: move randomly to another cell
    - Attack: move to the player

  - Generated Texture
    - Hands
    - Feet
    - Eyes
    - Body
    - Color

- Level
  - Simple aligned walls
  - 2D layout
  - Collision
  - Randomly generated
      1. Create outline
      2. Fill with maze
      3. Remove some walls with only one connection
      4. Decorate with textures

- Sound
  - Simple synthesized music
  - Simple synthesized sounds

- Hot reloadable during development

This should be enogh for a first publishable version.

# Compiling

Build all executables directly

```bash
./build.sh
```

Run with hot reloading

```bash
./out/hot ./src/main.c
```

With gdb

```bash
gdb --args ./out/hot ./src/main.c
```

Gdb does not reload the source automatically. Use the `dir` command for this.
