// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// generated.h - This file is generated by app/code_gen.c
#pragma once
typedef struct fmt_t fmt_t;
typedef struct gfx_vertex gfx_vertex;
typedef struct gfx_pass gfx_pass;
typedef struct gfx_atlas_item gfx_atlas_item;
typedef struct gl_t gl_t;
typedef struct gl_api gl_api;
typedef struct image image;
typedef struct buf buf;
typedef struct input_key input_key;
typedef struct input input;
static const char * const enum_to_str_input_key_code[] = {
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
typedef struct m4_single m4_single;
typedef struct m4 m4;
typedef struct mem mem;
typedef struct mem_page mem_page;
typedef struct os_dir os_dir;
static const char * const enum_to_str_Token_Type[] = {
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
typedef struct Parse Parse;
typedef struct sdl_win sdl_win;
typedef struct SDL_Keysym SDL_Keysym;
typedef struct SDL_CommonEvent SDL_CommonEvent;
typedef struct SDL_WindowEvent SDL_WindowEvent;
typedef struct SDL_KeyboardEvent SDL_KeyboardEvent;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;
typedef struct SDL_MouseButtonEvent SDL_MouseButtonEvent;
typedef struct SDL_MouseWheelEvent SDL_MouseWheelEvent;
typedef struct SDL_DropEvent SDL_DropEvent;
typedef union SDL_Event SDL_Event;
typedef struct sdl_api sdl_api;
typedef struct sound sound;
typedef struct Note Note;
typedef struct UI UI;
typedef struct Global Global;
typedef struct gen_enum gen_enum;
typedef struct hello hello;
typedef struct App App;
