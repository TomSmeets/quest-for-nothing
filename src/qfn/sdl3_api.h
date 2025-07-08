#pragma once
#include "lib/types.h"

typedef u32 SDL_InitFlags;
#define SDL_INIT_AUDIO 0x00000010u
#define SDL_INIT_VIDEO 0x00000020u
#define SDL_INIT_JOYSTICK 0x00000200u
#define SDL_INIT_HAPTIC 0x00001000u
#define SDL_INIT_GAMEPAD 0x00002000u
#define SDL_INIT_EVENTS 0x00004000u
#define SDL_INIT_SENSOR 0x00008000u
#define SDL_INIT_CAMERA 0x00010000u

bool SDL_InitSubSystem(SDL_InitFlags flags);
void SDL_Quit(void);
