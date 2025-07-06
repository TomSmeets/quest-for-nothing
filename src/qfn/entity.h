// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// entity.h - Generic object that exists in the game
#pragma once
#include "qfn/collision.h"
#include "qfn/image.h"
#include "qfn/mat.h"
#include "qfn/monster_sprite.h"

typedef enum {
    Entity_None,
    Entity_Player,  // Player
    Entity_Monster, // Ai
    Entity_Bullet,  //
    Entity_Wall,
} Entity_Type;
