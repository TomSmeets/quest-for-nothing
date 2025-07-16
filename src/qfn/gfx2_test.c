#include "gfx/gfx2.h"
#include "gfx/input.h"
#include "gfx/sound.h"
#include "gfx/sound_env.h"
#include "gfx/sound_music.h"
#include "gfx/sound_osc.h"
#include "gfx/sound_var.h"
#include "lib/id.h"
#include "lib/mutex.h"
#include "qfn/time.h"
#include "gfx/color_rand.h"

#if OS_IS_WASM
#include "gfx/gfx2_wasm.h"
#else
#include "gfx/gfx2_desktop.h"
#endif

struct App {
    Memory *mem;
    Gfx *gfx;
    Sound sound;

    bool volume1;
    bool volume2;
    f32 pitch;
    Mutex mutex;

    f32 angle;
    Image *images[64];
};

static void gfx_audio_callback(u32 sample_count, v2 *sample_list) {
    App *app = G->app;
    Sound *snd = &app->sound;

    mutex_lock(&app->mutex);
    for (u32 i = 0; i < sample_count; ++i) {
        sound_begin(snd);
        f32 v = 0;
        v += sound_pulse(snd, app->pitch, 0, .5) * sound_adsr(snd, app->volume1, 4, 10, .5);
        v += sound_saw(snd, app->pitch, 0) * sound_adsr(snd, app->volume2, 10, 4, .5);;
        Freeverb_Config cfg = {
            .room = 0.9f,
            .damp = 0.2f,
            .wet = 0.9f,
            .dry = 0.5f,
        };
        v2 vv;
        vv = sound_freeverb2(snd, cfg, v * 0.1) * 2.0f;
        vv = sound_clip2(vv);
        sample_list[i] = vv;
    }
    mutex_unlock(&app->mutex);
}

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->mem = mem;
    app->sound = sound_init(mem);
    app->gfx = gfx_init(mem, "GFX2 Test Application");

    for(u32 i = 0; i < array_count(app->images); ++i) {
        app->images[i] = image_new(mem, (v2u){32,32});
        image_fill(app->images[i], color4(color_rand_rainbow(G->rand)));
    }

    return app;
}

static void os_main(void) {
    // Initialize App
    if (!G->app) G->app = app_init();
    u64 t_start = os_time();
    f32 dt = 1.0f / 60.0f;

    Memory *tmp = mem_new();
    App *app = G->app;
    Input *input = gfx_begin(app->gfx);

    // Input handling
    if (input_click(input, KEY_G)) gfx_set_grab(app->gfx, !input->mouse_is_grabbed);
    if (input_click(input, KEY_F)) gfx_set_fullscreen(app->gfx, !input->is_fullscreen);
    if (input->quit || input_click(input, KEY_Q)) os_exit(0);

    if(input_click(input, KEY_R)) {
        for(u32 i = 0; i < array_count(app->images); ++i) {
            image_fill(app->images[i], color4(color_rand_rainbow(G->rand)));
            app->images[i]->id = id_next();
        }
    }

    // Music
    mutex_lock(&app->mutex);
    app->pitch = f_remap(input->mouse_pos.x, -input->window_size.x, input->window_size.x, NOTE_C, NOTE_C * 2);
    app->volume1 = input_down(input, KEY_MOUSE_LEFT);
    app->volume2 = input_down(input, KEY_MOUSE_RIGHT);
    mutex_unlock(&app->mutex);

    for (u32 i = 0; i < 1024; ++i) {
        f32 a = (1.0f + f_sqrt(5.0f)) / 2 * i * R4;

        Image *img = app->images[i % array_count(app->images)];

        m4 mtx = m4_id();
        m4_scale(&mtx, 0.2);
        m4_rotate_z(&mtx, app->angle);
        m4_translate_x(&mtx, 1);
        m4_rotate_z(&mtx, a);
        m4_translate_z(&mtx, i * -0.01f);
        gfx_draw(app->gfx, 1, mtx, img);
    }

    m4 camera = m4_id();
    m4_translate_z(&camera, -1);
    m4_rotate_y(&camera, R2);
    gfx_end(app->gfx, camera);

    app->angle += dt;

    mem_free(tmp);

    u64 t_end = os_time();
    u64 t_compute = t_end - t_start;
    u64 t_total = 1000 * 1000 * dt;
    u64 t_sleep = t_compute < t_total ? t_total - t_compute : 0;
    // fmt_su(G->fmt, "compute = ", t_compute, "\n");
    // fmt_su(G->fmt, "sleep  = ", t_sleep, "\n");
    G->os->sleep_time = t_sleep;
}
