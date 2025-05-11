// sdl_api.h - SDL2 api header, only what we use to improve compilation speed
#pragma once
#include "os_main.h"
#include "types.h"

#if 0
#include <SDL2/SDL.h>
#else
#define SDL_RELEASED 0
#define SDL_PRESSED 1
#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 3
#define SDL_BUTTON_X1 4
#define SDL_BUTTON_X2 5
#define SDL_WINDOWPOS_UNDEFINED 0x1FFF0000u

typedef u32 SDL_Keycode;
typedef u32 SDL_Scancode;

typedef enum {
    SDL_WINDOWEVENT_NONE,
    SDL_WINDOWEVENT_SHOWN,
    SDL_WINDOWEVENT_HIDDEN,
    SDL_WINDOWEVENT_EXPOSED,

    SDL_WINDOWEVENT_MOVED,

    SDL_WINDOWEVENT_RESIZED,
    SDL_WINDOWEVENT_SIZE_CHANGED,

    SDL_WINDOWEVENT_MINIMIZED,
    SDL_WINDOWEVENT_MAXIMIZED,
    SDL_WINDOWEVENT_RESTORED,

    SDL_WINDOWEVENT_ENTER,
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_GAINED,
    SDL_WINDOWEVENT_FOCUS_LOST,
    SDL_WINDOWEVENT_CLOSE,
    SDL_WINDOWEVENT_TAKE_FOCUS,
    SDL_WINDOWEVENT_HIT_TEST,
    SDL_WINDOWEVENT_ICCPROF_CHANGED,
    SDL_WINDOWEVENT_DISPLAY_CHANGED,
} SDL_WindowEventID;

typedef struct {
    SDL_Scancode scancode;
    SDL_Keycode sym;
    u16 mod;
    u32 unused;
} SDL_Keysym;

typedef enum {
    SDL_FIRSTEVENT = 0,
    SDL_QUIT = 0x100,

    SDL_WINDOWEVENT = 0x200,
    SDL_SYSWMEVENT,

    SDL_KEYDOWN = 0x300,
    SDL_KEYUP,
    SDL_TEXTEDITING,
    SDL_TEXTINPUT,
    SDL_KEYMAPCHANGED,
    SDL_TEXTEDITING_EXT,

    SDL_MOUSEMOTION = 0x400,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEWHEEL,

    SDL_JOYAXISMOTION = 0x600,
    SDL_JOYBALLMOTION,
    SDL_JOYHATMOTION,
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
    SDL_JOYDEVICEADDED,
    SDL_JOYDEVICEREMOVED,
    SDL_JOYBATTERYUPDATED,

    SDL_CONTROLLERAXISMOTION = 0x650,
    SDL_CONTROLLERBUTTONDOWN,
    SDL_CONTROLLERBUTTONUP,
    SDL_CONTROLLERDEVICEADDED,
    SDL_CONTROLLERDEVICEREMOVED,
    SDL_CONTROLLERDEVICEREMAPPED,
    SDL_CONTROLLERTOUCHPADDOWN,
    SDL_CONTROLLERTOUCHPADMOTION,
    SDL_CONTROLLERTOUCHPADUP,
    SDL_CONTROLLERSENSORUPDATE,

    SDL_FINGERDOWN = 0x700,
    SDL_FINGERUP,
    SDL_FINGERMOTION,

    SDL_DOLLARGESTURE = 0x800,
    SDL_DOLLARRECORD,
    SDL_MULTIGESTURE,

    SDL_CLIPBOARDUPDATE = 0x900,

    SDL_DROPFILE = 0x1000,
    SDL_DROPTEXT,
    SDL_DROPBEGIN,
    SDL_DROPCOMPLETE,

    SDL_AUDIODEVICEADDED = 0x1100,
    SDL_AUDIODEVICEREMOVED,

    SDL_SENSORUPDATE = 0x1200,

    SDL_RENDER_TARGETS_RESET = 0x2000,
    SDL_RENDER_DEVICE_RESET,
    SDL_POLLSENTINEL = 0x7F00,
    SDL_USEREVENT = 0x8000,
    SDL_LASTEVENT = 0xFFFF,
} SDL_EventType;

typedef struct {
    u32 type;
    u32 timestamp;
} SDL_CommonEvent;

typedef struct {
    u32 type;
    u32 timestamp;
    u32 windowID;
    u8 event;
    u8 padding1;
    u8 padding2;
    u8 padding3;
    i32 data1;
    i32 data2;
} SDL_WindowEvent;

typedef struct {
    u32 type;
    u32 timestamp;
    u32 windowID;
    u8 state;
    u8 repeat;
    u8 padding2;
    u8 padding3;
    SDL_Keysym keysym;
} SDL_KeyboardEvent;

typedef struct {
    u32 type;
    u32 timestamp;
    u32 windowID;
    u32 which;
    u32 state;
    i32 x;
    i32 y;
    i32 xrel;
    i32 yrel;
} SDL_MouseMotionEvent;

typedef struct {
    u32 type;
    u32 timestamp;
    u32 windowID;
    u32 which;
    u8 button;
    u8 state;
    u8 clicks;
    u8 padding1;
    i32 x;
    i32 y;
} SDL_MouseButtonEvent;

typedef struct {
    u32 type;
    u32 timestamp;
    u32 windowID;
    u32 which;
    i32 x;
    i32 y;
    u32 direction;
    float preciseX;
    float preciseY;
} SDL_MouseWheelEvent;

typedef struct {
    u32 type;
    u32 timestamp;
    char *file;
    u32 windowID;
} SDL_DropEvent;

typedef union {
    u32 type;
    SDL_WindowEvent window;
    SDL_KeyboardEvent key;
    SDL_MouseMotionEvent motion;
    SDL_MouseButtonEvent button;
    SDL_MouseWheelEvent wheel;
    SDL_DropEvent drop;
    u64 padding[8];
} SDL_Event;
static_assert(sizeof(SDL_Event) == 64);

#define SDL_INIT_EVERYTHING 0b1111001000110001u
typedef void SDL_Window;
typedef void SDL_GLContext;

typedef enum {
    SDL_GL_RED_SIZE,
    SDL_GL_GREEN_SIZE,
    SDL_GL_BLUE_SIZE,
    SDL_GL_ALPHA_SIZE,
    SDL_GL_BUFFER_SIZE,
    SDL_GL_DOUBLEBUFFER,
    SDL_GL_DEPTH_SIZE,
    SDL_GL_STENCIL_SIZE,
    SDL_GL_ACCUM_RED_SIZE,
    SDL_GL_ACCUM_GREEN_SIZE,
    SDL_GL_ACCUM_BLUE_SIZE,
    SDL_GL_ACCUM_ALPHA_SIZE,
    SDL_GL_STEREO,
    SDL_GL_MULTISAMPLEBUFFERS,
    SDL_GL_MULTISAMPLESAMPLES,
    SDL_GL_ACCELERATED_VISUAL,
    SDL_GL_RETAINED_BACKING,
    SDL_GL_CONTEXT_MAJOR_VERSION,
    SDL_GL_CONTEXT_MINOR_VERSION,
    SDL_GL_CONTEXT_EGL,
    SDL_GL_CONTEXT_FLAGS,
    SDL_GL_CONTEXT_PROFILE_MASK,
    SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
    SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
    SDL_GL_CONTEXT_RELEASE_BEHAVIOR,
    SDL_GL_CONTEXT_RESET_NOTIFICATION,
    SDL_GL_CONTEXT_NO_ERROR,
    SDL_GL_FLOATBUFFERS,
} SDL_GLattr;

typedef enum {
    SDL_GL_CONTEXT_DEBUG_FLAG = 0x0001,
    SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG = 0x0002,
    SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG = 0x0004,
    SDL_GL_CONTEXT_RESET_ISOLATION_FLAG = 0x0008,
} SDL_GLcontextFlag;

typedef enum {
    SDL_GL_CONTEXT_PROFILE_CORE = 0x0001,
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY = 0x0002,
    SDL_GL_CONTEXT_PROFILE_ES = 0x0004,
} SDL_GLprofile;

typedef struct {
    u32 format;
    int w;
    int h;
    int refresh_rate;
    void *driverdata;
} SDL_DisplayMode;

typedef enum {
    SDL_WINDOW_FULLSCREEN = 0x00000001,
    SDL_WINDOW_OPENGL = 0x00000002,
    SDL_WINDOW_SHOWN = 0x00000004,
    SDL_WINDOW_HIDDEN = 0x00000008,
    SDL_WINDOW_BORDERLESS = 0x00000010,
    SDL_WINDOW_RESIZABLE = 0x00000020,
    SDL_WINDOW_MINIMIZED = 0x00000040,
    SDL_WINDOW_MAXIMIZED = 0x00000080,
    SDL_WINDOW_MOUSE_GRABBED = 0x00000100,
    SDL_WINDOW_INPUT_FOCUS = 0x00000200,
    SDL_WINDOW_MOUSE_FOCUS = 0x00000400,
    SDL_WINDOW_FULLSCREEN_DESKTOP = (SDL_WINDOW_FULLSCREEN | 0x00001000),
    SDL_WINDOW_FOREIGN = 0x00000800,
    SDL_WINDOW_ALLOW_HIGHDPI = 0x00002000,
    SDL_WINDOW_MOUSE_CAPTURE = 0x00004000,
    SDL_WINDOW_ALWAYS_ON_TOP = 0x00008000,
    SDL_WINDOW_SKIP_TASKBAR = 0x00010000,
    SDL_WINDOW_UTILITY = 0x00020000,
    SDL_WINDOW_TOOLTIP = 0x00040000,
    SDL_WINDOW_POPUP_MENU = 0x00080000,
    SDL_WINDOW_KEYBOARD_GRABBED = 0x00100000,
    SDL_WINDOW_VULKAN = 0x10000000,
    SDL_WINDOW_METAL = 0x20000000,

    SDL_WINDOW_INPUT_GRABBED = SDL_WINDOW_MOUSE_GRABBED,
} SDL_WindowFlags;

typedef void (*SDL_AudioCallback)(void *userdata, u8 *stream, int len);
typedef u16 SDL_AudioFormat;

#define AUDIO_F32 0x8120

typedef struct SDL_AudioSpec {
    int freq;
    SDL_AudioFormat format;
    u8 channels;
    u8 silence;
    u16 samples;
    u16 padding;
    u32 size;
    SDL_AudioCallback callback;
    void *userdata;
} SDL_AudioSpec;

#define SDL_HINT_VIDEODRIVER "SDL_VIDEODRIVER"
#endif

typedef struct {
    // Core
    int (*SDL_Init)(u32 flags);
    int (*SDL_PollEvent)(SDL_Event *event);
    void (*SDL_Quit)(void);
    const char *(*SDL_GetError)(void);

    // Window
    SDL_Window *(*SDL_CreateWindow)(const char *title, int x, int y, int w, int h, u32 flags);
    int (*SDL_GetWindowDisplayMode)(SDL_Window *window, SDL_DisplayMode *mode);
    void (*SDL_GetWindowSize)(SDL_Window *window, int *w, int *h);
    bool (*SDL_SetHint)(const char *name, const char *value);
    int (*SDL_SetRelativeMouseMode)(int enabled);
    int (*SDL_SetWindowFullscreen)(SDL_Window *window, u32 flags);

    // OpenGL
    SDL_GLContext *(*SDL_GL_CreateContext)(SDL_Window *window);
    void *(*SDL_GL_GetProcAddress)(const char *proc);
    int (*SDL_GL_SetAttribute)(SDL_GLattr attr, int value);
    int (*SDL_GL_SetSwapInterval)(int interval);
    void (*SDL_GL_SwapWindow)(SDL_Window *window);

    // Audio
    void (*SDL_LockAudio)(void);
    int (*SDL_OpenAudio)(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
    void (*SDL_PauseAudio)(int pause_on);
    void (*SDL_UnlockAudio)(void);
} Sdl_Api;

static void sdl_api_load(Sdl_Api *api, File *handle) {
    // Core
    api->SDL_Init = os_dlsym(handle, "SDL_Init");
    api->SDL_PollEvent = os_dlsym(handle, "SDL_PollEvent");
    api->SDL_Quit = os_dlsym(handle, "SDL_Quit");
    api->SDL_GetError = os_dlsym(handle, "SDL_GetError");

    // Window
    api->SDL_CreateWindow = os_dlsym(handle, "SDL_CreateWindow");
    api->SDL_GetWindowDisplayMode = os_dlsym(handle, "SDL_GetWindowDisplayMode");
    api->SDL_GetWindowSize = os_dlsym(handle, "SDL_GetWindowSize");
    api->SDL_SetHint = os_dlsym(handle, "SDL_SetHint");
    api->SDL_SetRelativeMouseMode = os_dlsym(handle, "SDL_SetRelativeMouseMode");
    api->SDL_SetWindowFullscreen = os_dlsym(handle, "SDL_SetWindowFullscreen");

    // OpenGL
    api->SDL_GL_CreateContext = os_dlsym(handle, "SDL_GL_CreateContext");
    api->SDL_GL_GetProcAddress = os_dlsym(handle, "SDL_GL_GetProcAddress");
    api->SDL_GL_SetAttribute = os_dlsym(handle, "SDL_GL_SetAttribute");
    api->SDL_GL_SetSwapInterval = os_dlsym(handle, "SDL_GL_SetSwapInterval");
    api->SDL_GL_SwapWindow = os_dlsym(handle, "SDL_GL_SwapWindow");

    // Audio
    api->SDL_LockAudio = os_dlsym(handle, "SDL_LockAudio");
    api->SDL_OpenAudio = os_dlsym(handle, "SDL_OpenAudio");
    api->SDL_PauseAudio = os_dlsym(handle, "SDL_PauseAudio");
    api->SDL_UnlockAudio = os_dlsym(handle, "SDL_UnlockAudio");
}
