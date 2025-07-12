#include "gfx/gfx2.h"
#include "gfx/gfx2_desktop.h"
#include "gfx/sound_var.h"
#include "gfx/sound_osc.h"
#include "gfx/sound_music.h"
#include "gfx/sound.h"
#include "lib/mutex.h"

struct App {
    Memory *mem;
    Gfx *gfx;
    Sound sound;

    f32 volume1;
    f32 volume2;
    f32 pitch;
    Mutex mutex;
};

static void gfx_audio_callback(u32 sample_count, v2 *sample_list) {
    App *app = G->app;
    Sound *snd = &app->sound;

    mutex_lock(&app->mutex);
    for(u32 i = 0; i < sample_count; ++i) {
        sound_begin(snd);
        f32 v = 0;
        v += sound_sine(snd, app->pitch, 0) * app->volume1;
        v += sound_saw(snd,  app->pitch, 0) * app->volume2;
        v = sound_clip(v);
        sample_list[i] = (v2){ v, v };
    }
    mutex_unlock(&app->mutex);
}

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->mem = mem;
    app->sound = sound_init(mem);
    app->gfx = gfx_init(mem, "GFX2 Test Application");
    return app;
}

static void os_main(void) {
    // Initialize App
    if (!G->app) G->app = app_init();

    Memory *tmp = mem_new();
    App *app = G->app;
    Input *input = gfx_begin(app->gfx);

    // Input handling
    if(input_click(input, KEY_G)) gfx_set_grab(app->gfx, !input->mouse_is_grabbed);
    if(input_click(input, KEY_F)) gfx_set_fullscreen(app->gfx, !input->is_fullscreen);
    if(input_click(input, KEY_Q)) {
        gfx_quit(app->gfx);
        os_exit(0);
    }

    // Music
    mutex_lock(&app->mutex);
    app->pitch = f_remap(input->mouse_pos.x, -input->window_size.x, input->window_size.x, NOTE_C, NOTE_C*2);
    app->volume1 = input_down(input, KEY_MOUSE_LEFT) ? 1 : 0;
    app->volume2 = input_down(input, KEY_MOUSE_RIGHT) ? 1 : 0;
    mutex_unlock(&app->mutex);

    Image *img = image_new(tmp, (v2u){16,16});
    m4 mtx = m4_id();
    gfx_draw(app->gfx, false, mtx, img);

    m4 camera = m4_id();
    gfx_end(app->gfx, camera);

    mem_free(tmp);
}
