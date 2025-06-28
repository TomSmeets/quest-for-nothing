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
#include "qfn/gun.h"
#include "qfn/image.h"
#include "qfn/level.h"
#include "qfn/monster.h"
#include "qfn/player.h"
#include "qfn/sparse_set.h"
#include "qfn/wall.h"

/*
Game Design V1.0
- Only one hit-scan gun
- Monsters creep towards you and try to eat you
- Monsters are generated pixel art sprites
- Levels are generated
- World is 2d maze with walls
    - grid aligned
- single player
- walls can be painted
*/

typedef struct {
    Memory *mem;

    Player *player;
    Monster *monster_list;
    Wall *walls;

    Game_Debug debug;
    Sparse_Set *sparse;
    Audio audio;
} Game;

static void game_gen_monsters(Game *game, Rand *rng, v3i spawn) {
    Sprite_Properties s1 = sprite_new(rng);
    Sprite_Properties s2 = sprite_new(rng);

    // Generate player
    game->player = player_new(game->mem, v3i_to_v3(spawn));

    for (Wall *wall = game->walls; wall; wall = wall->next) {
        // Only consider floor tiles
        if (wall->mtx.z.y < 0.5) continue;

        v3 pos = wall->mtx.w;

        // Don't generate them too close
        f32 spawn_area = 4;
        if (v3_distance_sq(pos, game->player->pos) < spawn_area * spawn_area) continue;

        // Choose random sprite props
        Sprite_Properties prop = s1;
        if (rand_choice(rng, 0.5)) prop = s2;

        Monster *mon = monster_new(game->mem, pos, prop);
        mon->next = game->monster_list;
        game->monster_list = mon;
    }
}

// Create a new game
static Game *game_new(Rand *rng) {
    v2i level_size = {8, 8};
    v2i spawn = 0;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;

    // Create Level
    game->walls = level_generate(mem, rng, level_size);

    // Generate Monsters
    game_gen_monsters(game, rng, (v3i){spawn.x, 0, spawn.y});

    game->sparse = sparse_set_new(mem);
    game->audio.snd = sound_init(mem);

    Image *img = image_new(mem, (v2u){32, 32});
    image_fill(img, (v4){1, 0, 1, 1});
    return game;
}

static void game_update(Game *game, Engine *eng) {
    Collision_World *world = collision_world_new(eng->tmp);

    // Toggle debug drawing
    if (key_click(eng->input, KEY_4)) {
        debug_next(&game->debug);
    }

    for (Wall *wall = game->walls; wall; wall = wall->next) {
        wall_update(wall, eng, world);
    }

    for (Monster *mon = game->monster_list; mon; mon = mon->next) {
        monster_update(mon, eng, &game->audio, world, game->player->pos);
    }

    player_update(game->player, world, eng, &game->audio);

    // Update bvh
    sparse_set_swap(game->sparse);
}
