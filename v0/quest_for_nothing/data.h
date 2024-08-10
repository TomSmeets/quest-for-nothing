#pragma once
#include "tlib/mat.h"
#include "tlib/mem.h"
#include "tlib/global.h"
#include "tlib/sound.h"

struct Monster {
    v3 pos;
    f32 radius;
    f32 height;

    bool has_target;
    v3 target_pos;
    f32 life;
    f32 angle;

    Image *img;

    Monster *next;
};

struct Player {
    v3 pos;
    f32 pitch;
    f32 yaw;

    m4 view_to_world;
    m4 world_to_clip;

    v3 old_pos;
    bool on_ground;
    bool can_jump_again;
};

struct Level {
    mem mem;
    Player  *player;
    Monster *monster_list;

    f32 time;
    f32 next_monster_spawn_time;
};

struct App {
    mem tmp;
    mem perm;
    Global global;

    Sdl *window;

    f32 dt;
    f32 time;

    // Frame Limiter
    u64 dt_us;
    u64 time_us;
    u64 start_time;

    // Animation
    f32 t;

    gl_t *gl;
    UI *ui;

    Image *img;
    Player player;

    Sound_System sound;
    rand_t rng;

    f32 next_monster_spawn_time;

    Level *level;
};
