// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// generated.h - This file is generated by app/code_gen.c
#pragma once
typedef struct fmt_t fmt_t;
typedef struct Gfx_Vertex Gfx_Vertex;
typedef struct Gfx Gfx;
typedef struct gfx_pass gfx_pass;
typedef struct gfx_atlas_item gfx_atlas_item;
typedef struct gl_t gl_t;
typedef struct gl_api gl_api;
typedef struct Global Global;
typedef struct image image;
typedef struct buf buf;
typedef struct input_key input_key;
typedef struct Input Input;
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
typedef struct Rect Rect;
typedef struct UI UI;
typedef struct UI_Component UI_Component;
typedef struct gen_enum gen_enum;
typedef struct hello hello;
typedef struct App App;
#define GL_SHADER_VERT_SIZE 649
static const unsigned char GL_SHADER_VERT_DATA[] = {
    47,47,32,67,111,112,121,114,105,103,104,116,32,40,99,41,
    32,50,48,50,51,32,45,32,84,111,109,32,83,109,101,101,
    116,115,32,60,116,111,109,64,116,115,109,101,101,116,115,46,
    110,108,62,10,47,47,32,103,108,95,115,104,97,100,101,114,
    46,118,101,114,116,32,45,32,65,32,115,105,109,112,108,101,
    32,79,112,101,110,71,76,32,118,101,114,116,101,120,32,83,
    104,97,100,101,114,10,35,118,101,114,115,105,111,110,32,51,
    51,48,32,99,111,114,101,10,10,47,47,32,86,101,114,116,
    101,120,32,97,116,116,114,105,98,117,116,101,115,44,32,116,
    104,105,115,32,105,110,102,111,114,109,97,116,105,111,110,32,
    105,115,32,112,97,115,115,101,100,32,100,105,114,101,99,116,
    108,121,32,102,114,111,109,32,116,104,101,32,99,112,117,10,
    108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,
    61,32,48,41,32,105,110,32,118,101,99,51,32,97,95,112,
    111,115,59,10,108,97,121,111,117,116,40,108,111,99,97,116,
    105,111,110,32,61,32,49,41,32,105,110,32,118,101,99,50,
    32,97,95,117,118,59,10,10,47,47,32,79,117,116,112,117,
    116,115,32,119,101,32,115,101,110,100,32,116,111,32,116,104,
    101,32,102,114,97,103,109,101,110,116,32,115,104,97,100,101,
    114,10,111,117,116,32,118,101,99,50,32,118,95,117,118,59,
    10,10,47,47,32,85,110,105,102,111,114,109,115,32,97,114,
    101,32,112,97,115,115,101,100,32,100,105,114,101,99,116,108,
    121,32,102,114,111,109,32,116,104,101,32,99,112,117,32,112,
    101,114,32,100,114,97,119,32,99,97,108,108,10,47,47,32,
    84,104,105,115,32,109,97,116,114,105,120,32,116,114,97,110,
    115,102,111,114,109,115,32,119,111,114,108,100,32,112,111,115,
    105,116,105,111,110,115,32,105,110,116,111,32,115,99,114,101,
    101,110,32,115,112,97,99,101,32,99,111,111,114,100,105,110,
    97,116,101,115,10,117,110,105,102,111,114,109,32,109,97,116,
    52,32,109,97,116,59,10,10,118,111,105,100,32,109,97,105,
    110,40,41,32,123,10,32,32,32,32,47,47,32,65,112,112,
    108,121,32,116,104,101,32,116,114,97,110,115,102,111,114,109,
    97,116,105,111,110,32,109,97,116,114,105,120,10,32,32,32,
    32,103,108,95,80,111,115,105,116,105,111,110,32,61,32,109,
    97,116,32,42,32,118,101,99,52,40,97,95,112,111,115,44,
    32,49,46,48,41,59,10,10,32,32,32,32,47,47,32,100,
    105,114,101,99,116,108,121,32,102,111,114,119,97,114,100,32,
    117,118,32,99,111,111,114,100,105,110,97,116,101,115,32,116,
    111,32,116,104,101,32,102,114,97,103,109,101,110,116,32,115,
    104,97,100,101,114,10,32,32,32,32,118,95,117,118,32,61,
    32,97,95,117,118,59,10,125,10,0,
};
#define GL_SHADER_FRAG_SIZE 806
static const unsigned char GL_SHADER_FRAG_DATA[] = {
    47,47,32,67,111,112,121,114,105,103,104,116,32,40,99,41,
    32,50,48,50,51,32,45,32,84,111,109,32,83,109,101,101,
    116,115,32,60,116,111,109,64,116,115,109,101,101,116,115,46,
    110,108,62,10,47,47,32,103,108,95,115,104,97,100,101,114,
    46,102,114,97,103,32,45,32,65,32,115,105,109,112,108,101,
    32,79,112,101,110,71,76,32,102,114,97,103,109,101,110,116,
    32,83,104,97,100,101,114,10,35,118,101,114,115,105,111,110,
    32,51,51,48,32,99,111,114,101,10,10,47,47,32,84,104,
    101,32,102,105,110,97,108,32,99,111,108,111,114,32,100,114,
    97,119,110,32,116,111,32,116,104,101,32,115,99,114,101,101,
    110,10,111,117,116,32,118,101,99,52,32,111,117,116,95,99,
    111,108,111,114,59,10,10,47,47,32,84,104,101,32,116,101,
    120,116,117,114,101,32,97,116,108,97,115,32,119,101,32,99,
    97,110,32,115,97,109,112,108,101,32,102,114,111,109,10,117,
    110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,
    32,105,109,103,59,10,10,47,47,32,73,110,116,101,114,112,
    111,108,97,116,101,100,32,118,97,108,117,101,115,32,115,101,
    110,116,32,102,114,111,109,32,116,104,101,32,118,101,114,116,
    101,120,32,115,104,97,100,101,114,10,105,110,32,118,101,99,
    50,32,118,95,117,118,59,10,105,110,32,102,108,111,97,116,
    32,118,95,99,111,108,111,114,59,10,10,118,111,105,100,32,
    109,97,105,110,40,41,32,123,10,32,32,32,32,47,47,32,
    76,111,111,107,117,112,32,116,104,101,32,99,111,108,111,114,
    32,105,110,32,116,104,101,32,116,101,120,116,117,114,101,32,
    97,116,108,97,115,10,32,32,32,32,111,117,116,95,99,111,
    108,111,114,32,61,32,116,101,120,116,117,114,101,40,105,109,
    103,44,32,118,95,117,118,41,59,10,10,32,32,32,32,47,
    47,32,76,105,109,105,116,32,116,104,101,32,99,111,108,111,
    114,32,116,111,32,91,48,45,49,93,10,32,32,32,32,47,
    47,32,65,108,108,32,99,111,108,111,114,115,32,97,114,101,
    32,105,110,32,91,48,45,105,110,102,93,32,114,97,110,103,
    101,44,32,116,104,105,115,32,97,108,108,111,119,115,32,117,
    115,32,116,111,32,104,97,118,101,32,118,101,114,121,32,98,
    114,105,103,104,116,32,108,105,103,104,116,115,10,32,32,32,
    32,47,47,32,73,110,32,116,104,101,32,102,117,116,117,114,
    101,32,119,101,32,119,97,110,116,32,116,111,32,100,111,32,
    115,111,109,101,32,72,68,82,32,115,116,117,102,102,32,115,
    117,99,104,32,97,115,32,98,108,111,111,109,46,10,32,32,
    32,32,111,117,116,95,99,111,108,111,114,46,114,103,98,32,
    47,61,32,109,97,120,40,108,101,110,103,116,104,40,111,117,
    116,95,99,111,108,111,114,46,114,103,98,41,44,32,49,41,
    59,10,10,32,32,32,32,47,47,32,111,117,116,95,99,111,
    108,111,114,32,61,32,118,101,99,52,40,118,101,99,51,40,
    118,95,117,118,44,32,49,41,42,46,53,32,43,32,111,117,
    116,95,99,111,108,111,114,46,114,103,98,42,46,53,44,32,
    49,41,59,10,10,32,32,32,32,47,47,32,68,105,115,99,
    97,114,100,32,116,114,97,110,115,112,97,114,101,110,116,32,
    112,105,120,101,108,115,10,32,32,32,32,105,102,32,40,111,
    117,116,95,99,111,108,111,114,46,97,32,60,61,32,48,46,
    49,41,10,32,32,32,32,32,32,32,32,100,105,115,99,97,
    114,100,59,10,125,10,0,
};
