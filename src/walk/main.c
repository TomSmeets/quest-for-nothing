#include "gfx/gfx.h"
#include "gfx/image.h"
#include "gfx/input.h"
#include "walk/lander.h"
#include "walk/net.h"
#include "walk/verlet.h"

struct App {
    Gfx *gfx;

    Body *body;
    Lander *lander;

    u32 iteration;
    Net_Network *network;

    Image *img_vertex;
    Image *img_spring;
    Image *img_muscle;
    Vertex *select;
};

static void gfx_audio_callback(u32 sample_count, v2 *sample_list) {
}

static Body *app_body_new(Memory *mem) {
    Body *body = body_new(mem);

    // Head / Body
    Vertex *p1 = body_vertex(body, (v2){0, 2 + 2});
    Vertex *p2 = body_vertex(body, (v2){4, 2 + 2});
    Vertex *p3 = body_vertex(body, (v2){0, 1 + 2});
    Vertex *p4 = body_vertex(body, (v2){4, 1 + 2});
    body_spring(body, p1, p2);
    body_spring(body, p2, p4);
    body_spring(body, p4, p3);
    body_spring(body, p3, p1);
    body_spring(body, p1, p4);
    body_spring(body, p2, p3);

    // Legs
    Vertex *p5 = body_vertex(body, (v2){0, 2});
    Vertex *p6 = body_vertex(body, (v2){4, 2});
    Vertex *p7 = body_vertex(body, (v2){0, 1});
    Vertex *p8 = body_vertex(body, (v2){4, 1});
    body_spring(body, p3, p5);
    body_spring(body, p4, p6);
    body_spring(body, p5, p7);
    body_spring(body, p6, p8);

    // Muscle
    body_muscle(body, p5, p3, p4);
    body_muscle(body, p6, p4, p3);
    body_muscle(body, p3, p5, p7);
    body_muscle(body, p4, p6, p8);
    return body;
}

static Net_Network *app_net_new(Memory *mem) {
    // Create nerual network
    Net_Network *network = mem_struct(mem, Net_Network);
    net_push_layer(network, mem, 3, Activation_None);
    net_push_layer(network, mem, 8, Activation_Relu);
    net_push_layer(network, mem, 8, Activation_Relu);
    net_push_layer(network, mem, 1, Activation_Sigmoid);
    return network;
}

static Image *img_gen_sphere(Memory *mem, u32 size, v3 color) {
    Image *img = image_new(mem, (v2u){size, size});
    for (u32 y = 0; y < size; ++y) {
        for (u32 x = 0; x < size; ++x) {
            v4 v = {color.x, color.y, color.z, 1};
            f32 fx = ((f32)x + 0.5f) / (f32)size * 2 - 1;
            f32 fy = ((f32)y + 0.5f) / (f32)size * 2 - 1;
            if (fx * fx + fy * fy > 1) v.w = 0;
            image_write4(img, (v2i){x, y}, v);
        }
    }
    return img;
}

static Image *img_gen_square(Memory *mem, u32 size, v3 color) {
    Image *img = image_new(mem, (v2u){size, size});
    image_fill(img, (v4){color.x, color.y, color.z, 1});
    return img;
}

static Lander *app_lander_new(Memory *mem) {
    Lander *lander = mem_struct(mem, Lander);
    lander->fuel = 2;
    return lander;
}

static App *app_load(void) {
    if (G->app) return G->app;
    Memory *mem = G->mem;

    App *app = G->app = mem_struct(mem, App);

    app->gfx = gfx_init(mem, "Walking Evolution");
    app->body = app_body_new(mem);
    app->network = app_net_new(mem);
    app->lander = app_lander_new(mem);
    app->img_vertex = img_gen_sphere(mem, 8, COLOR_RED);
    app->img_spring = img_gen_square(mem, 8, COLOR_BLACK);
    app->img_muscle = img_gen_square(mem, 8, (v3){0.2, 0, 0});
    return app;
}

static m4 mtx_line(v2 a, v2 b) {
    v2 dir = b - a;
    f32 s = 0.05;
    v2 x = dir;
    v2 y = s * v2_rot90(v2_normalize(dir));
    v2 z = 1;
    v2 w = (a + b) / 2;
    m4 mtx = {
        {x.x, x.y, 0},
        {y.x, y.y, 0},
        {z.x, z.y, 0},
        {w.x, w.y, 0},
    };
    return mtx;
}

static void gfx_line(Gfx *gfx, Image *img, v2 a, v2 b) {
    gfx_draw_3d(gfx, mtx_line(a, b), img);
}

static void os_main() {
    App *app = app_load();
    Input *input = gfx_begin(app->gfx);
    if (input->quit) os_exit(0);

    f32 a1 = 0;
    f32 a2 = 0;
    f32 a3 = 0;
    f32 a4 = 0;
    f32 s = R1;

    if (input_down(input, KEY_A)) a1 += s;
    if (input_down(input, KEY_D)) a1 -= s;
    if (input_down(input, KEY_W)) a2 += s;
    if (input_down(input, KEY_S)) a2 -= s;
    if (input_down(input, KEY_J)) a3 -= s;
    if (input_down(input, KEY_L)) a3 += s;
    if (input_down(input, KEY_I)) a4 -= s;
    if (input_down(input, KEY_K)) a4 += s;

    app->body->muscle->angle_target = a4;
    app->body->muscle->next->angle_target = a2;
    app->body->muscle->next->next->angle_target = a3;
    app->body->muscle->next->next->next->angle_target = a1;

    v2 mouse = {input->mouse_pos.x, input->mouse_pos.y};
    mouse /= input->window_size.x;
    mouse *= 20.5;
    mouse += (v2){2, 3};

    if (input_click(input, KEY_MOUSE_LEFT)) {
        f32 min_dist = .1;
        Vertex *select = 0;
        for (Vertex *vtx = app->body->vtx; vtx; vtx = vtx->next) {
            f32 d = v2_length_sq(vtx->pos - mouse);
            if (d > min_dist) continue;
            min_dist = d;
            select = vtx;
        }
        app->select = select;
    }

    if (!input_down(input, KEY_MOUSE_LEFT)) {
        app->select = 0;
    }

    {
        m4 mtx = m4_id();
        m4_scale(&mtx, 0.1);
        m4_translate(&mtx, (v3){mouse.x, mouse.y, 0});
        gfx_draw_3d(app->gfx, mtx, app->img_vertex);
    }

    u32 n = 4;
    for (u32 i = 0; i < n; ++i) {
        if (app->select) {
            app->select->pos = mouse;
            app->select->old = mouse;
        }
        verlet_step(app->body, G->dt / n);
    }

    for (Vertex *vtx = app->body->vtx; vtx; vtx = vtx->next) {
        m4 mtx = m4_id();
        m4_scale(&mtx, 0.1 + vtx->pressure * 0.1);
        m4_translate(&mtx, (v3){vtx->pos.x, vtx->pos.y, 0});
        gfx_draw_3d(app->gfx, mtx, app->img_vertex);
    }

    for (Spring *spring = app->body->spring; spring; spring = spring->next) {
        v2 a = spring->vtx_a->pos;
        v2 b = spring->vtx_b->pos;
        gfx_line(app->gfx, app->img_spring, a, b);
    }

    for (Muscle *muscle = app->body->muscle; muscle; muscle = muscle->next) {
        v2 a = muscle->vtx_a->pos;
        v2 b = muscle->vtx_b->pos;
        v2 c = muscle->vtx_c->pos;
        gfx_line(app->gfx, app->img_muscle, (a + b) / 2, (b + c) / 2);
    }

    {
        f32 thrust = input_down(input, KEY_SPACE);

        app->network->layer_first->activation[0] = app->lander->pos;
        app->network->layer_first->activation[1] = app->lander->old;
        app->network->layer_first->activation[2] = app->lander->fuel;
        net_forward(app->network);
        thrust = app->network->layer_last->activation[0];
        lander_step(app->lander, G->dt, thrust);
        fmt_s(G->fmt, "Fuel: ");
        fmt_f(G->fmt, app->lander->fuel);
        fmt_s(G->fmt, " ");
        fmt_s(G->fmt, "Score: ");
        fmt_f(G->fmt, app->lander->score);
        fmt_s(G->fmt, " ");
        fmt_s(G->fmt, "Input: ");
        fmt_f(G->fmt, thrust);
        fmt_s(G->fmt, "\n");

        m4 mtx = m4_id();
        m4_translate_y(&mtx, app->lander->pos);
        gfx_draw_3d(app->gfx, mtx, app->img_vertex);
    }

    m4 cam = m4_id();
    m4_rotate_y(&cam, R2);
    m4_translate(&cam, (v3){2, 3, 8});
    gfx_end(app->gfx, (v3){.2, .3, .4}, cam);
    // os_sleep(1000 * 100);
}
