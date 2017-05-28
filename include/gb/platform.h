/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#ifndef  GB_PLATFORM_H__
# define GB_PLATFORM_H__

#include "gb/types.h"

GB_DEF void gb_exit(uint32_t code);

GB_DEF void gb_yield(void);

GB_DEF void gb_set_env(char const *name, char const *value);

GB_DEF void gb_unset_env(char const *name);

GB_DEF uint16_t gb_endian_swap16(uint16_t i);

GB_DEF uint32_t gb_endian_swap32(uint32_t i);

GB_DEF uint64_t gb_endian_swap64(uint64_t i);

GB_DEF ssize_t gb_count_set_bits(uint64_t mask);

#if defined(GB_PLATFORM)

// NOTE(bill):
// Coordiate system - +ve x - left to right
//                  - +ve y - bottom to top
//                  - Relative to window

// TODO(bill): Proper documentation for this with code examples

// Window Support - Complete
// OS X Support - Missing:
//     * Sofware framebuffer
//     * (show|hide) window
//     * show_cursor
//     * toggle (fullscreen|borderless)
//     * set window position
//     * Clipboard
//     * GameControllers
// Linux Support - None
// Other OS Support - None

#ifndef GB_MAX_GAME_CONTROLLER_COUNT
#define GB_MAX_GAME_CONTROLLER_COUNT 4
#endif

typedef enum gbKeyType {
  gbKey_Unknown = 0,  // Unhandled key

  // NOTE(bill): Allow the basic printable keys to be aliased with their chars
  gbKey_0 = '0',
  gbKey_1,
  gbKey_2,
  gbKey_3,
  gbKey_4,
  gbKey_5,
  gbKey_6,
  gbKey_7,
  gbKey_8,
  gbKey_9,

  gbKey_A = 'A',
  gbKey_B,
  gbKey_C,
  gbKey_D,
  gbKey_E,
  gbKey_F,
  gbKey_G,
  gbKey_H,
  gbKey_I,
  gbKey_J,
  gbKey_K,
  gbKey_L,
  gbKey_M,
  gbKey_N,
  gbKey_O,
  gbKey_P,
  gbKey_Q,
  gbKey_R,
  gbKey_S,
  gbKey_T,
  gbKey_U,
  gbKey_V,
  gbKey_W,
  gbKey_X,
  gbKey_Y,
  gbKey_Z,

  gbKey_Lbracket  = '[',
  gbKey_Rbracket  = ']',
  gbKey_Semicolon = ';',
  gbKey_Comma     = ',',
  gbKey_Period    = '.',
  gbKey_Quote     = '\'',
  gbKey_Slash     = '/',
  gbKey_Backslash = '\\',
  gbKey_Grave     = '`',
  gbKey_Equals    = '=',
  gbKey_Minus     = '-',
  gbKey_Space     = ' ',

  gbKey__Pad = 128,   // NOTE(bill): make sure ASCII is reserved

  gbKey_Escape,       // Escape
  gbKey_Lcontrol,     // Left Control
  gbKey_Lshift,       // Left Shift
  gbKey_Lalt,         // Left Alt
  gbKey_Lsystem,      // Left OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
  gbKey_Rcontrol,     // Right Control
  gbKey_Rshift,       // Right Shift
  gbKey_Ralt,         // Right Alt
  gbKey_Rsystem,      // Right OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
  gbKey_Menu,         // Menu
  gbKey_Return,       // Return
  gbKey_Backspace,    // Backspace
  gbKey_Tab,          // Tabulation
  gbKey_Pageup,       // Page up
  gbKey_Pagedown,     // Page down
  gbKey_End,          // End
  gbKey_Home,         // Home
  gbKey_Insert,       // Insert
  gbKey_Delete,       // Delete
  gbKey_Plus,         // +
  gbKey_Subtract,     // -
  gbKey_Multiply,     // *
  gbKey_Divide,       // /
  gbKey_Left,         // Left arrow
  gbKey_Right,        // Right arrow
  gbKey_Up,           // Up arrow
  gbKey_Down,         // Down arrow
  gbKey_Numpad0,      // Numpad 0
  gbKey_Numpad1,      // Numpad 1
  gbKey_Numpad2,      // Numpad 2
  gbKey_Numpad3,      // Numpad 3
  gbKey_Numpad4,      // Numpad 4
  gbKey_Numpad5,      // Numpad 5
  gbKey_Numpad6,      // Numpad 6
  gbKey_Numpad7,      // Numpad 7
  gbKey_Numpad8,      // Numpad 8
  gbKey_Numpad9,      // Numpad 9
  gbKey_NumpadDot,    // Numpad .
  gbKey_NumpadEnter,  // Numpad Enter
  gbKey_F1,           // F1
  gbKey_F2,           // F2
  gbKey_F3,           // F3
  gbKey_F4,           // F4
  gbKey_F5,           // F5
  gbKey_F6,           // F6
  gbKey_F7,           // F7
  gbKey_F8,           // F8
  gbKey_F9,           // F8
  gbKey_F10,          // F10
  gbKey_F11,          // F11
  gbKey_F12,          // F12
  gbKey_F13,          // F13
  gbKey_F14,          // F14
  gbKey_F15,          // F15
  gbKey_Pause,        // Pause

  gbKey_Count,
} gbKeyType;

/* TODO(bill): Change name? */
typedef uint8_t gbKeyState;
typedef enum gbKeyStateFlag {
  gbKeyState_Down     = GB_BIT(0),
  gbKeyState_Pressed  = GB_BIT(1),
  gbKeyState_Released = GB_BIT(2)
} gbKeyStateFlag;

GB_DEF void gb_key_state_update(gbKeyState *s, byte32_t is_down);

typedef enum gbMouseButtonType {
  gbMouseButton_Left,
  gbMouseButton_Middle,
  gbMouseButton_Right,
  gbMouseButton_X1,
  gbMouseButton_X2,

  gbMouseButton_Count
} gbMouseButtonType;

typedef enum gbControllerAxisType {
  gbControllerAxis_LeftX,
  gbControllerAxis_LeftY,
  gbControllerAxis_RightX,
  gbControllerAxis_RightY,
  gbControllerAxis_LeftTrigger,
  gbControllerAxis_RightTrigger,

  gbControllerAxis_Count
} gbControllerAxisType;

typedef enum gbControllerButtonType {
  gbControllerButton_Up,
  gbControllerButton_Down,
  gbControllerButton_Left,
  gbControllerButton_Right,
  gbControllerButton_A,
  gbControllerButton_B,
  gbControllerButton_X,
  gbControllerButton_Y,
  gbControllerButton_LeftShoulder,
  gbControllerButton_RightShoulder,
  gbControllerButton_Back,
  gbControllerButton_Start,
  gbControllerButton_LeftThumb,
  gbControllerButton_RightThumb,

  gbControllerButton_Count
} gbControllerButtonType;

typedef struct gbGameController {
  byte16_t is_connected, is_analog;

  float32_t        axes[gbControllerAxis_Count];
  gbKeyState buttons[gbControllerButton_Count];
} gbGameController;

#if defined(GB_SYSTEM_WINDOWS)
  typedef struct _XINPUT_GAMEPAD XINPUT_GAMEPAD;
  typedef struct _XINPUT_STATE   XINPUT_STATE;
  typedef struct _XINPUT_VIBRATION XINPUT_VIBRATION;

#define GB_XINPUT_GET_STATE(name) unsigned long __stdcall name(unsigned long dwUserIndex, XINPUT_STATE *pState)
  typedef GB_XINPUT_GET_STATE(gbXInputGetStateProc);

#define GB_XINPUT_SET_STATE(name) unsigned long __stdcall name(unsigned long dwUserIndex, XINPUT_VIBRATION *pVibration)
  typedef GB_XINPUT_SET_STATE(gbXInputSetStateProc);
#endif


typedef enum gbWindowFlag {
  gbWindow_Fullscreen        = GB_BIT(0),
  gbWindow_Hidden            = GB_BIT(1),
  gbWindow_Borderless        = GB_BIT(2),
  gbWindow_Resizable         = GB_BIT(3),
  gbWindow_Minimized         = GB_BIT(4),
  gbWindow_Maximized         = GB_BIT(5),
  gbWindow_FullscreenDesktop = gbWindow_Fullscreen | gbWindow_Borderless,
} gbWindowFlag;

typedef enum gbRendererType {
  gbRenderer_Opengl,
  gbRenderer_Software,

  gbRenderer_Count,
} gbRendererType;



#if defined(GB_SYSTEM_WINDOWS) && !defined(_WINDOWS_)
typedef struct tagBITMAPINFOHEADER {
  unsigned long biSize;
  long          biWidth;
  long          biHeight;
  uint16_t           biPlanes;
  uint16_t           biBitCount;
  unsigned long biCompression;
  unsigned long biSizeImage;
  long          biXPelsPerMeter;
  long          biYPelsPerMeter;
  unsigned long biClrUsed;
  unsigned long biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagRGBQUAD {
  uint8_t rgbBlue;
  uint8_t rgbGreen;
  uint8_t rgbRed;
  uint8_t rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;
#endif

typedef struct gbPlatform {
  byte32_t is_initialized;

  void *window_handle;
  int32_t   window_x, window_y;
  int32_t   window_width, window_height;
  uint32_t   window_flags;
  byte16_t   window_is_closed, window_has_focus;

#if defined(GB_SYSTEM_WINDOWS)
  void *win32_dc;
#elif defined(GB_SYSTEM_OSX)
  void *osx_autorelease_pool; // TODO(bill): Is this really needed?
#endif

  gbRendererType renderer_type;
  union {
    struct {
      void *      context;
      int32_t         major;
      int32_t         minor;
      byte16_t         core, compatible;
      gbDllHandle dll_handle;
    } opengl;

    // NOTE(bill): Software rendering
    struct {
#if defined(GB_SYSTEM_WINDOWS)
      BITMAPINFO win32_bmi;
#endif
      void *     memory;
      ssize_t      memory_size;
      int32_t        pitch;
      int32_t        bits_per_pixel;
    } sw_framebuffer;
  };

  gbKeyState keys[gbKey_Count];
  struct {
    gbKeyState control;
    gbKeyState alt;
    gbKeyState shift;
  } key_modifiers;

  rune_t  char_buffer[256];
  ssize_t char_buffer_count;

  byte32_t mouse_clip;
  int32_t mouse_x, mouse_y;
  int32_t mouse_dx, mouse_dy; // NOTE(bill): Not raw mouse movement
  int32_t mouse_raw_dx, mouse_raw_dy; // NOTE(bill): Raw mouse movement
  float32_t mouse_wheel_delta;
  gbKeyState mouse_buttons[gbMouseButton_Count];

  gbGameController game_controllers[GB_MAX_GAME_CONTROLLER_COUNT];

  float64_t              curr_time;
  float64_t              dt_for_frame;
  byte32_t              quit_requested;

#if defined(GB_SYSTEM_WINDOWS)
  struct {
    gbXInputGetStateProc *get_state;
    gbXInputSetStateProc *set_state;
  } xinput;
#endif
} gbPlatform;


typedef struct gbVideoMode {
  int32_t width, height;
  int32_t bits_per_pixel;
} gbVideoMode;

GB_DEF gbVideoMode gb_video_mode                     (int32_t width, int32_t height, int32_t bits_per_pixel);
GB_DEF byte32_t         gb_video_mode_is_valid            (gbVideoMode mode);
GB_DEF gbVideoMode gb_video_mode_get_desktop         (void);
GB_DEF ssize_t       gb_video_mode_get_fullscreen_modes(gbVideoMode *modes, ssize_t max_mode_count); // NOTE(bill): returns mode count
GB_DEF GB_COMPARE_PROC(gb_video_mode_cmp);     // NOTE(bill): Sort smallest to largest (Ascending)
GB_DEF GB_COMPARE_PROC(gb_video_mode_dsc_cmp); // NOTE(bill): Sort largest to smallest (Descending)


// NOTE(bill): Software rendering
GB_DEF byte32_t   gb_platform_init_with_software         (gbPlatform *p, char const *window_title, int32_t width, int32_t height, uint32_t window_flags);
// NOTE(bill): OpenGL Rendering
GB_DEF byte32_t   gb_platform_init_with_opengl           (gbPlatform *p, char const *window_title, int32_t width, int32_t height, uint32_t window_flags, int32_t major, int32_t minor, byte32_t core, byte32_t compatible);
GB_DEF void  gb_platform_update                     (gbPlatform *p);
GB_DEF void  gb_platform_display                    (gbPlatform *p);
GB_DEF void  gb_platform_destroy                    (gbPlatform *p);
GB_DEF void  gb_platform_show_cursor                (gbPlatform *p, byte32_t show);
GB_DEF void  gb_platform_set_mouse_position         (gbPlatform *p, int32_t x, int32_t y);
GB_DEF void  gb_platform_set_controller_vibration   (gbPlatform *p, ssize_t index, float32_t left_motor, float32_t right_motor);
GB_DEF byte32_t   gb_platform_has_clipboard_text         (gbPlatform *p);
GB_DEF void  gb_platform_set_clipboard_text         (gbPlatform *p, char const *str);
GB_DEF char *gb_platform_get_clipboard_text         (gbPlatform *p, gbAllocator a);
GB_DEF void  gb_platform_set_window_position        (gbPlatform *p, int32_t x, int32_t y);
GB_DEF void  gb_platform_set_window_title           (gbPlatform *p, char const *title, ...) GB_PRINTF_ARGS(2);
GB_DEF void  gb_platform_toggle_fullscreen          (gbPlatform *p, byte32_t fullscreen_desktop);
GB_DEF void  gb_platform_toggle_borderless          (gbPlatform *p);
GB_DEF void  gb_platform_make_opengl_context_current(gbPlatform *p);
GB_DEF void  gb_platform_show_window                (gbPlatform *p);
GB_DEF void  gb_platform_hide_window                (gbPlatform *p);


#endif // GB_PLATFORM

#endif /* GB_PLATFORM_H__ */
