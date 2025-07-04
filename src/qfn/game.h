// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "lib/mem.h"
#include "lib/rand.h"
#include "lib/types.h"
#include "lib/vec.h"
#include "qfn/audio.h"
#include "qfn/collision.h"
#include "qfn/engine.h"
#include "qfn/game_debug.h"
#include "qfn/level.h"
#include "qfn/monster.h"
#include "qfn/player.h"
#include "qfn/wall.h"

typedef struct {
    Memory *mem;

    Player *player;
    Monster *monster_list;
    Wall *walls;

    Game_Debug debug;
    Audio audio;
} Game;

static Monster *game_gen_monsters(Memory *mem, Wall *walls, Rand *rng, v3 spawn) {
    Sprite_Properties s1 = sprite_new(rng);
    Sprite_Properties s2 = sprite_new(rng);

    Monster *monster_list = 0;
    for (Wall *wall = walls; wall; wall = wall->next) {

        // Only consider walls that point up (floor)
        if (wall->mtx.z.y < 0.5) continue;

        // Choose random sprite props
        Sprite_Properties prop = s1;
        if (rand_choice(rng, 0.5)) prop = s2;

        u32 n = rand_u32(rng, 0, 4);
        for (u32 i = 0; i < n; ++i) {
            v3 pos = wall->mtx.w;
            pos.x += rand_f32(rng, -1, 1);
            pos.z += rand_f32(rng, -1, 1);

            // Don't generate them too close
            f32 spawn_area = 4;
            if (v3_distance_sq(pos, spawn) < spawn_area * spawn_area) continue;

            Monster *mon = monster_new(mem, pos, prop);
            mon->next = monster_list;
            monster_list = mon;
        }
    }
    return monster_list;
}

// Create a new game
static Game *game_new(Rand *rng) {
    v2i level_size = {8, 8};
    v3 spawn = 0;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;
    game->walls = level_generate(mem, rng, level_size);
    game->monster_list = game_gen_monsters(mem, game->walls, rng, spawn);
    game->player = player_new(game->mem, spawn);
    game->audio.snd = sound_init(mem);
    return game;
}

static void game_update(Game *game, Engine *eng) {
    Collision_World *world = collision_world_new(eng->tmp);

    for (Wall *wall = game->walls; wall; wall = wall->next) {
        wall_update(wall, eng, world);
    }

    for (Monster *mon = game->monster_list; mon; mon = mon->next) {
        monster_update(mon, eng, &game->audio, world, game->player->pos);
    }

    player_update(game->player, world, eng, &game->audio);
}
