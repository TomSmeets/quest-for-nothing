#pragma once
// ==== src/color.h ====
// ==== src/fmt.h ====
typedef struct fmt_t fmt_t;
// ==== src/gfx.h ====
typedef struct gfx_vertex gfx_vertex;
typedef struct gfx_pass gfx_pass;
// ==== src/gl.h ====
// ==== src/gl_api.h ====
typedef struct gl_api gl_api;
// ==== src/gl_shader.frag ====
// ==== src/gl_shader.vert ====
// ==== src/image.h ====
typedef struct image image;
// ==== src/inc.h ====
typedef struct buf buf;
// ==== src/input.h ====
typedef struct input_key input_key;
typedef struct input input;
static const char *enum_to_str_input_key_code[] = {
    "KEY_NONE",
    "KEY_0",
    "KEY_1",
    "KEY_2",
    "KEY_3",
    "KEY_4",
    "KEY_5",
    "KEY_6",
    "KEY_7",
    "KEY_8",
    "KEY_9",
    "KEY_A",
    "KEY_B",
    "KEY_C",
    "KEY_D",
    "KEY_E",
    "KEY_F",
    "KEY_G",
    "KEY_H",
    "KEY_I",
    "KEY_J",
    "KEY_K",
    "KEY_L",
    "KEY_M",
    "KEY_N",
    "KEY_O",
    "KEY_P",
    "KEY_Q",
    "KEY_R",
    "KEY_S",
    "KEY_T",
    "KEY_U",
    "KEY_V",
    "KEY_W",
    "KEY_X",
    "KEY_Y",
    "KEY_Z",
    "KEY_MINUS",
    "KEY_EQUALS",
    "KEY_SPACE",
    "KEY_SEMICOLON",
    "KEY_SHIFT",
    "KEY_MOUSE_LEFT",
    "KEY_MOUSE_MIDDLE",
    "KEY_MOUSE_RIGHT",
    "PAD_A",
    "PAD_B",
    "PAD_X",
    "PAD_Y",
    "PAD_BUMPER_LEFT",
    "PAD_BUMPER_RIGHT",
    "PAD_TRIGGER_LEFT",
    "PAD_TRIGGER_RIGHT",
    "PAD_UP",
    "PAD_DOWN",
    "PAD_LEFT",
    "PAD_RIGHT",
    "KEY_UP",
    "KEY_DOWN",
    "KEY_LEFT",
    "KEY_RIGHT",
    "KEY_COUNT",
};
// ==== src/khrplatform.h ====
// ==== src/mat.h ====
typedef struct m4_single m4_single;
typedef struct m4 m4;
// ==== src/math.h ====
// ==== src/mem.h ====
typedef struct mem mem;
// ==== src/os.h ====
typedef struct mem_page mem_page;
typedef struct os_dir os_dir;
// ==== src/os_generic.h ====
// ==== src/os_linux.h ====
// ==== src/os_windows.h ====
// ==== src/parse.h ====
static const char *enum_to_str_Token_Type[] = {
    "Token_None",
    "Token_Symbol",
    "Token_Number",
    "Token_String",
    "Token_Char",
    "Token_Comment",
    "Token_Op",
    "Token_Macro",
};
typedef struct Token Token;
// ==== src/sdl.h ====
typedef struct sdl_win sdl_win;
// ==== src/sdl_api.h ====
typedef struct SDL_Keysym SDL_Keysym;
typedef struct SDL_CommonEvent SDL_CommonEvent;
typedef struct SDL_WindowEvent SDL_WindowEvent;
typedef struct sdl_api sdl_api;
// ==== src/sound.h ====
typedef struct sound sound;
// ==== src/str.h ====
// ==== src/typedefs.h ====
// ==== src/vec.h ====
// ==== src/ui.h ====
typedef struct UI UI;
// ==== src/global.h ====
typedef struct Global Global;
// ==== src/os_mem_caching.h ====
// ==== src/generated.h ====
// ==== app/code_gen.c ====
typedef struct gen_enum gen_enum;
// ==== app/hello.c ====
typedef struct hello hello;
// ==== app/hot.c ====
// ==== app/parse.c ====
typedef struct Parse Parse;
typedef struct Token Token;
// ==== app/quest_for_nothing.c ====
