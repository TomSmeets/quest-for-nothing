#pragma once

#include "app/quest_for_nothing/data.h"
#include "tlib/vec.h"
#include "tlib/gfx_draw.h"

static void monster_draw(Monster *mon, Gfx *gfx) {
    gfx_image(gfx, mon->img);
    gfx_color(gfx, (v4){1, 1, 1, 1});
    gfx_rect(gfx, (v2){0,0}, (v2){1,1});
}


static void mon_update(App *app, Monster *mon, Gfx *gfx) {
    bool is_alive = mon->life > 0;

    v3 player_dir   = app->player.pos - mon->pos;
    f32 player_dist = v3_len(player_dir);
    player_dir.z = 0;

    if(is_alive)
        mon->angle = f_atan2(player_dir.y, player_dir.x);

    m4 mtx = m4_id();
    m4_trans(&mtx, (v3){-.5, 0, 0.001});
    if(is_alive) m4_rot_x(&mtx, R1);
    m4_rot_z(&mtx, mon->angle + R1);
    m4_trans(&mtx, mon->pos); // move into position
    gfx->mtx = mtx;

    mon->img = app->img;
    monster_draw(mon, gfx);


    if(is_alive) {
        if(player_dist < 4.0) {
            mon->pos -= player_dir / (player_dist*player_dist) * app->dt;
        }
        
        if(player_dist < 0.5) {
            snd_play_squish(&app->sound);
            mon->life = 0;
        }
    
        v3 move_dir = mon->target_pos - mon->pos;
        f32 move_len = v3_len(move_dir);
        v3 move_dir_norm = move_dir / move_len;
        if(move_len < 0.5 || !mon->has_target) {
            mon->has_target = 1;
            mon->target_pos = rand_v2(&app->rng)*20;
        } else {
            mon->pos += move_dir_norm*app->dt*.25;
        }
    }

    // Gravity
    if(mon->pos.z > 0) {
        mon->pos.z -= app->dt;
    } else {
        mon->pos.z = 0;
    }
}
