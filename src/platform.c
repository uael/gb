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

#include "gb/platform.h"

#if defined(GB_SYSTEM_WINDOWS)
gb_inline void gb_exit(u32 code) { ExitProcess(code); }
#else
gb_inline void gb_exit(u32 code) { exit(code); }
#endif

gb_inline void gb_yield(void) {
#if defined(GB_SYSTEM_WINDOWS)
  Sleep(0);
#else
  sched_yield();
#endif
}

gb_inline void gb_set_env(char const *name, char const *value) {
#if defined(GB_SYSTEM_WINDOWS)
  // TODO(bill): Should this be a Wide version?
	SetEnvironmentVariableA(name, value);
#else
  setenv(name, value, 1);
#endif
}

gb_inline void gb_unset_env(char const *name) {
#if defined(GB_SYSTEM_WINDOWS)
  // TODO(bill): Should this be a Wide version?
	SetEnvironmentVariableA(name, NULL);
#else
  unsetenv(name);
#endif
}


gb_inline u16 gb_endian_swap16(u16 i) {
  return (i>>8) | (i<<8);
}

gb_inline u32 gb_endian_swap32(u32 i) {
  return (i>>24) |(i<<24) |
         ((i&0x00ff0000u)>>8)  | ((i&0x0000ff00u)<<8);
}

gb_inline u64 gb_endian_swap64(u64 i) {
  return (i>>56) | (i<<56) |
         ((i&0x00ff000000000000ull)>>40) | ((i&0x000000000000ff00ull)<<40) |
         ((i&0x0000ff0000000000ull)>>24) | ((i&0x0000000000ff0000ull)<<24) |
         ((i&0x000000ff00000000ull)>>8)  | ((i&0x00000000ff000000ull)<<8);
}


gb_inline isize gb_count_set_bits(u64 mask) {
  isize count = 0;
  while (mask) {
    count += (mask & 1);
    mask >>= 1;
  }
  return count;
}






////////////////////////////////////////////////////////////////
//
// Platform
//
//

#if defined(GB_PLATFORM)

gb_inline void gb_key_state_update(gbKeyState *s, b32 is_down) {
	b32 was_down = (*s & gbKeyState_Down) != 0;
	is_down = is_down != 0; // NOTE(bill): Make sure it's a boolean
	GB_MASK_SET(*s, is_down,               gbKeyState_Down);
	GB_MASK_SET(*s, !was_down &&  is_down, gbKeyState_Pressed);
	GB_MASK_SET(*s,  was_down && !is_down, gbKeyState_Released);
}

#if defined(GB_SYSTEM_WINDOWS)

#ifndef ERROR_DEVICE_NOT_CONNECTED
#define ERROR_DEVICE_NOT_CONNECTED 1167
#endif

GB_XINPUT_GET_STATE(gbXInputGetState_Stub) {
	gb_unused(dwUserIndex); gb_unused(pState);
	return ERROR_DEVICE_NOT_CONNECTED;
}
GB_XINPUT_SET_STATE(gbXInputSetState_Stub) {
	gb_unused(dwUserIndex); gb_unused(pVibration);
	return ERROR_DEVICE_NOT_CONNECTED;
}


gb_internal gb_inline f32 gb__process_xinput_stick_value(i16 value, i16 dead_zone_threshold) {
	f32 result = 0;

	if (value < -dead_zone_threshold)
		result = cast(f32) (value + dead_zone_threshold) / (32768.0f - dead_zone_threshold);
	else if (value > dead_zone_threshold)
		result = cast(f32) (value - dead_zone_threshold) / (32767.0f - dead_zone_threshold);

	return result;
}

gb_internal void gb__platform_resize_dib_section(gbPlatform *p, i32 width, i32 height) {
	if ((p->renderer_type == gbRenderer_Software) &&
	    !(p->window_width == width && p->window_height == height)) {
		BITMAPINFO bmi = {0};

		if (width == 0 || height == 0)
			return;

		p->window_width  = width;
		p->window_height = height;

		// TODO(bill): Is this slow to get the desktop mode everytime?
		p->sw_framebuffer.bits_per_pixel = gb_video_mode_get_desktop().bits_per_pixel;
		p->sw_framebuffer.pitch = (p->sw_framebuffer.bits_per_pixel * width / 8);

		bmi.bmiHeader.biSize = gb_size_of(bmi.bmiHeader);
		bmi.bmiHeader.biWidth       = width;
		bmi.bmiHeader.biHeight      = height; // NOTE(bill): -ve is top-down, +ve is bottom-up
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = cast(u16)p->sw_framebuffer.bits_per_pixel;
		bmi.bmiHeader.biCompression = 0 /*BI_RGB*/;

		p->sw_framebuffer.win32_bmi = bmi;


		if (p->sw_framebuffer.memory)
			gb_vm_free(gb_virtual_memory(p->sw_framebuffer.memory, p->sw_framebuffer.memory_size));

		{
			isize memory_size = p->sw_framebuffer.pitch * height;
			gbVirtualMemory vm = gb_vm_alloc(0, memory_size);
			p->sw_framebuffer.memory      = vm.data;
			p->sw_framebuffer.memory_size = vm.size;
		}
	}
}


gb_internal gbKeyType gb__win32_from_vk(unsigned int key) {
	// NOTE(bill): Letters and numbers are defined the same for VK_* and GB_*
	if (key >= 'A' && key < 'Z') return cast(gbKeyType)key;
	if (key >= '0' && key < '9') return cast(gbKeyType)key;
	switch (key) {
	case VK_ESCAPE: return gbKey_Escape;

	case VK_LCONTROL: return gbKey_Lcontrol;
	case VK_LSHIFT:   return gbKey_Lshift;
	case VK_LMENU:    return gbKey_Lalt;
	case VK_LWIN:     return gbKey_Lsystem;
	case VK_RCONTROL: return gbKey_Rcontrol;
	case VK_RSHIFT:   return gbKey_Rshift;
	case VK_RMENU:    return gbKey_Ralt;
	case VK_RWIN:     return gbKey_Rsystem;
	case VK_MENU:     return gbKey_Menu;

	case VK_OEM_4:      return gbKey_Lbracket;
	case VK_OEM_6:      return gbKey_Rbracket;
	case VK_OEM_1:      return gbKey_Semicolon;
	case VK_OEM_COMMA:  return gbKey_Comma;
	case VK_OEM_PERIOD: return gbKey_Period;
	case VK_OEM_7:      return gbKey_Quote;
	case VK_OEM_2:      return gbKey_Slash;
	case VK_OEM_5:      return gbKey_Backslash;
	case VK_OEM_3:      return gbKey_Grave;
	case VK_OEM_PLUS:   return gbKey_Equals;
	case VK_OEM_MINUS:  return gbKey_Minus;

	case VK_SPACE:  return gbKey_Space;
	case VK_RETURN: return gbKey_Return;
	case VK_BACK:   return gbKey_Backspace;
	case VK_TAB:    return gbKey_Tab;

	case VK_PRIOR:  return gbKey_Pageup;
	case VK_NEXT:   return gbKey_Pagedown;
	case VK_END:    return gbKey_End;
	case VK_HOME:   return gbKey_Home;
	case VK_INSERT: return gbKey_Insert;
	case VK_DELETE: return gbKey_Delete;

	case VK_ADD:      return gbKey_Plus;
	case VK_SUBTRACT: return gbKey_Subtract;
	case VK_MULTIPLY: return gbKey_Multiply;
	case VK_DIVIDE:   return gbKey_Divide;

	case VK_LEFT:  return gbKey_Left;
	case VK_RIGHT: return gbKey_Right;
	case VK_UP:    return gbKey_Up;
	case VK_DOWN:  return gbKey_Down;

	case VK_NUMPAD0:   return gbKey_Numpad0;
	case VK_NUMPAD1:   return gbKey_Numpad1;
	case VK_NUMPAD2:   return gbKey_Numpad2;
	case VK_NUMPAD3:   return gbKey_Numpad3;
	case VK_NUMPAD4:   return gbKey_Numpad4;
	case VK_NUMPAD5:   return gbKey_Numpad5;
	case VK_NUMPAD6:   return gbKey_Numpad6;
	case VK_NUMPAD7:   return gbKey_Numpad7;
	case VK_NUMPAD8:   return gbKey_Numpad8;
	case VK_NUMPAD9:   return gbKey_Numpad9;
	case VK_SEPARATOR: return gbKey_NumpadEnter;
	case VK_DECIMAL:   return gbKey_NumpadDot;

	case VK_F1:  return gbKey_F1;
	case VK_F2:  return gbKey_F2;
	case VK_F3:  return gbKey_F3;
	case VK_F4:  return gbKey_F4;
	case VK_F5:  return gbKey_F5;
	case VK_F6:  return gbKey_F6;
	case VK_F7:  return gbKey_F7;
	case VK_F8:  return gbKey_F8;
	case VK_F9:  return gbKey_F9;
	case VK_F10: return gbKey_F10;
	case VK_F11: return gbKey_F11;
	case VK_F12: return gbKey_F12;
	case VK_F13: return gbKey_F13;
	case VK_F14: return gbKey_F14;
	case VK_F15: return gbKey_F15;

	case VK_PAUSE: return gbKey_Pause;
	}
	return gbKey_Unknown;
}
LRESULT CALLBACK gb__win32_window_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// NOTE(bill): Silly callbacks
	gbPlatform *platform = cast(gbPlatform *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	b32 window_has_focus = (platform != NULL) && platform->window_has_focus;

	if (msg == WM_CREATE) { // NOTE(bill): Doesn't need the platform
		// NOTE(bill): https://msdn.microsoft.com/en-us/library/windows/desktop/ms645536(v=vs.85).aspx
		RAWINPUTDEVICE rid[2] = {0};

		// NOTE(bill): Keyboard
		rid[0].usUsagePage = 0x01;
		rid[0].usUsage     = 0x06;
		rid[0].dwFlags     = 0x00000030/*RIDEV_NOLEGACY*/; // NOTE(bill): Do not generate legacy messages such as WM_KEYDOWN
		rid[0].hwndTarget  = hWnd;

		// NOTE(bill): Mouse
		rid[1].usUsagePage = 0x01;
		rid[1].usUsage     = 0x02;
		rid[1].dwFlags     = 0; // NOTE(bill): adds HID mouse and also allows legacy mouse messages to allow for window movement etc.
		rid[1].hwndTarget  = hWnd;

		if (RegisterRawInputDevices(rid, gb_count_of(rid), gb_size_of(rid[0])) == false) {
			DWORD err = GetLastError();
			GB_PANIC("Failed to initialize raw input device for win32."
			         "Err: %u", err);
		}
	}

	if (!platform)
		return DefWindowProcW(hWnd, msg, wParam, lParam);

	switch (msg) {
	case WM_CLOSE:
	case WM_DESTROY:
		platform->window_is_closed = true;
		return 0;

	case WM_QUIT: {
		platform->quit_requested = true;
	} break;

	case WM_UNICHAR: {
		if (window_has_focus) {
			if (wParam == '\r')
				wParam = '\n';
			// TODO(bill): Does this need to be thread-safe?
			platform->char_buffer[platform->char_buffer_count++] = cast(Rune)wParam;
		}
	} break;


	case WM_INPUT: {
		RAWINPUT raw = {0};
		unsigned int size = gb_size_of(RAWINPUT);

		if (!GetRawInputData(cast(HRAWINPUT)lParam, RID_INPUT, &raw, &size, gb_size_of(RAWINPUTHEADER))) {
			return 0;
		}
		switch (raw.header.dwType) {
		case RIM_TYPEKEYBOARD: {
			// NOTE(bill): Many thanks to https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
			// for the
			RAWKEYBOARD *raw_kb = &raw.data.keyboard;
			unsigned int vk = raw_kb->VKey;
			unsigned int scan_code = raw_kb->MakeCode;
			unsigned int flags = raw_kb->Flags;
			// NOTE(bill): e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
			// NOTE(bill): http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
			b32 is_e0   = (flags & RI_KEY_E0) != 0;
			b32 is_e1   = (flags & RI_KEY_E1) != 0;
			b32 is_up   = (flags & RI_KEY_BREAK) != 0;
			b32 is_down = !is_up;

			// TODO(bill): Should I handle scan codes?

			if (vk == 255) {
				// NOTE(bill): Discard "fake keys"
				return 0;
			} else if (vk == VK_SHIFT) {
				// NOTE(bill): Correct left/right shift
				vk = MapVirtualKeyW(scan_code, MAPVK_VSC_TO_VK_EX);
			} else if (vk == VK_NUMLOCK) {
				// NOTE(bill): Correct PAUSE/BREAK and NUM LOCK and set the extended bit
				scan_code = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC) | 0x100;
			}

			if (is_e1) {
				// NOTE(bill): Escaped sequences, turn vk into the correct scan code
				// except for VK_PAUSE (it's a bug)
				if (vk == VK_PAUSE)
					scan_code = 0x45;
				else
					scan_code = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
			}

			switch (vk) {
			case VK_CONTROL: vk = (is_e0) ? VK_RCONTROL : VK_LCONTROL; break;
			case VK_MENU:    vk = (is_e0) ? VK_RMENU    : VK_LMENU;   break;

			case VK_RETURN: if (is_e0)  vk = VK_SEPARATOR; break; // NOTE(bill): Numpad return
			case VK_DELETE: if (!is_e0) vk = VK_DECIMAL;   break; // NOTE(bill): Numpad dot
			case VK_INSERT: if (!is_e0) vk = VK_NUMPAD0;   break;
			case VK_HOME:   if (!is_e0) vk = VK_NUMPAD7;   break;
			case VK_END:    if (!is_e0) vk = VK_NUMPAD1;   break;
			case VK_PRIOR:  if (!is_e0) vk = VK_NUMPAD9;   break;
			case VK_NEXT:   if (!is_e0) vk = VK_NUMPAD3;   break;

			// NOTE(bill): The standard arrow keys will always have their e0 bit set, but the
			// corresponding keys on the NUMPAD will not.
			case VK_LEFT:  if (!is_e0) vk = VK_NUMPAD4; break;
			case VK_RIGHT: if (!is_e0) vk = VK_NUMPAD6; break;
			case VK_UP:    if (!is_e0) vk = VK_NUMPAD8; break;
			case VK_DOWN:  if (!is_e0) vk = VK_NUMPAD2; break;

			// NUMPAD 5 doesn't have its e0 bit set
			case VK_CLEAR: if (!is_e0) vk = VK_NUMPAD5; break;
			}

			// NOTE(bill): Set appropriate key state flags
			gb_key_state_update(&platform->keys[gb__win32_from_vk(vk)], is_down);

		} break;
		case RIM_TYPEMOUSE: {
			RAWMOUSE *raw_mouse = &raw.data.mouse;
			u16 flags = raw_mouse->usButtonFlags;
			long dx = +raw_mouse->lLastX;
			long dy = -raw_mouse->lLastY;

			if (flags & RI_MOUSE_WHEEL)
				platform->mouse_wheel_delta = cast(i16)raw_mouse->usButtonData;

			platform->mouse_raw_dx = dx;
			platform->mouse_raw_dy = dy;
		} break;
		}
	} break;

	default: break;
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}


typedef void *wglCreateContextAttribsARB_Proc(void *hDC, void *hshareContext, int const *attribList);


b32 gb__platform_init(gbPlatform *p, char const *window_title, gbVideoMode mode, gbRendererType type, u32 window_flags) {
	WNDCLASSEXW wc = {gb_size_of(WNDCLASSEXW)};
	DWORD ex_style = 0, style = 0;
	RECT wr;
	u16 title_buffer[256] = {0}; // TODO(bill): gb_local_persist this?

	wc.style = CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC
	wc.lpfnWndProc   = gb__win32_window_callback;
	wc.hbrBackground = cast(HBRUSH)GetStockObject(0/*WHITE_BRUSH*/);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"gb-win32-wndclass"; // TODO(bill): Is this enough?
	wc.hInstance     = GetModuleHandleW(NULL);

	if (RegisterClassExW(&wc) == 0) {
		MessageBoxW(NULL, L"Failed to register the window class", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if ((window_flags & gbWindow_Fullscreen) && !(window_flags & gbWindow_Borderless)) {
		DEVMODEW screen_settings = {gb_size_of(DEVMODEW)};
		screen_settings.dmPelsWidth	 = mode.width;
		screen_settings.dmPelsHeight = mode.height;
		screen_settings.dmBitsPerPel = mode.bits_per_pixel;
		screen_settings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettingsW(&screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			if (MessageBoxW(NULL, L"The requested fullscreen mode is not supported by\n"
			                L"your video card. Use windowed mode instead?",
			                L"",
			                MB_YESNO|MB_ICONEXCLAMATION) == IDYES) {
				window_flags &= ~gbWindow_Fullscreen;
			} else {
				mode = gb_video_mode_get_desktop();
				screen_settings.dmPelsWidth	 = mode.width;
				screen_settings.dmPelsHeight = mode.height;
				screen_settings.dmBitsPerPel = mode.bits_per_pixel;
				ChangeDisplaySettingsW(&screen_settings, CDS_FULLSCREEN);
			}
		}
	}


	// ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	// style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

	style |= WS_VISIBLE;

	if (window_flags & gbWindow_Hidden)       style &= ~WS_VISIBLE;
	if (window_flags & gbWindow_Resizable)    style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
	if (window_flags & gbWindow_Maximized)    style |=  WS_MAXIMIZE;
	if (window_flags & gbWindow_Minimized)    style |=  WS_MINIMIZE;

	// NOTE(bill): Completely ignore the given mode and just change it
	if (window_flags & gbWindow_FullscreenDesktop) {
		mode = gb_video_mode_get_desktop();
	}

	if ((window_flags & gbWindow_Fullscreen) || (window_flags & gbWindow_Borderless)) {
		style |= WS_POPUP;
	} else {
		style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}


	wr.left   = 0;
	wr.top    = 0;
	wr.right  = mode.width;
	wr.bottom = mode.height;
	AdjustWindowRect(&wr, style, false);

	p->window_flags  = window_flags;
	p->window_handle = CreateWindowExW(ex_style,
	                                   wc.lpszClassName,
	                                   cast(wchar_t const *)gb_utf8_to_ucs2(title_buffer, gb_size_of(title_buffer), window_title),
	                                   style,
	                                   CW_USEDEFAULT, CW_USEDEFAULT,
	                                   wr.right - wr.left, wr.bottom - wr.top,
	                                   0, 0,
	                                   GetModuleHandleW(NULL),
	                                   NULL);

	if (!p->window_handle) {
		MessageBoxW(NULL, L"Window creation failed", L"Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	p->win32_dc = GetDC(cast(HWND)p->window_handle);

	p->renderer_type = type;
	switch (p->renderer_type) {
	case gbRenderer_Opengl: {
		wglCreateContextAttribsARB_Proc *wglCreateContextAttribsARB;
		i32 attribs[8] = {0};
		isize c = 0;

		PIXELFORMATDESCRIPTOR pfd = {gb_size_of(PIXELFORMATDESCRIPTOR)};
		pfd.nVersion     = 1;
		pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType   = PFD_TYPE_RGBA;
		pfd.cColorBits   = 32;
		pfd.cAlphaBits   = 8;
		pfd.cDepthBits   = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType   = PFD_MAIN_PLANE;

		SetPixelFormat(cast(HDC)p->win32_dc, ChoosePixelFormat(cast(HDC)p->win32_dc, &pfd), NULL);
		p->opengl.context = cast(void *)wglCreateContext(cast(HDC)p->win32_dc);
		wglMakeCurrent(cast(HDC)p->win32_dc, cast(HGLRC)p->opengl.context);

		if (p->opengl.major > 0) {
			attribs[c++] = 0x2091; // WGL_CONTEXT_MAJOR_VERSION_ARB
			attribs[c++] = gb_max(p->opengl.major, 1);
		}
		if (p->opengl.major > 0 && p->opengl.minor >= 0) {
			attribs[c++] = 0x2092; // WGL_CONTEXT_MINOR_VERSION_ARB
			attribs[c++] = gb_max(p->opengl.minor, 0);
		}

		if (p->opengl.core) {
			attribs[c++] = 0x9126; // WGL_CONTEXT_PROFILE_MASK_ARB
			attribs[c++] = 0x0001; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
		} else if (p->opengl.compatible) {
			attribs[c++] = 0x9126; // WGL_CONTEXT_PROFILE_MASK_ARB
			attribs[c++] = 0x0002; // WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
		}
		attribs[c++] = 0; // NOTE(bill): tells the proc that this is the end of attribs

		wglCreateContextAttribsARB = cast(wglCreateContextAttribsARB_Proc *)wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARB) {
			HGLRC rc = cast(HGLRC)wglCreateContextAttribsARB(p->win32_dc, 0, attribs);
			if (rc && wglMakeCurrent(cast(HDC)p->win32_dc, rc)) {
				p->opengl.context = rc;
			} else {
				// TODO(bill): Handle errors from GetLastError
				// ERROR_INVALID_VERSION_ARB 0x2095
				// ERROR_INVALID_PROFILE_ARB 0x2096
			}
		}

	} break;

	case gbRenderer_Software:
		gb__platform_resize_dib_section(p, mode.width, mode.height);
		break;

	default:
		GB_PANIC("Unknown window type");
		break;
	}

	SetForegroundWindow(cast(HWND)p->window_handle);
	SetFocus(cast(HWND)p->window_handle);
	SetWindowLongPtrW(cast(HWND)p->window_handle, GWLP_USERDATA, cast(LONG_PTR)p);

	p->window_width  = mode.width;
	p->window_height = mode.height;

	if (p->renderer_type == gbRenderer_Opengl) {
		p->opengl.dll_handle = gb_dll_load("opengl32.dll");
	}

	{ // Load XInput
		// TODO(bill): What other dlls should I look for?
		gbDllHandle xinput_library = gb_dll_load("xinput1_4.dll");
		p->xinput.get_state = gbXInputGetState_Stub;
		p->xinput.set_state = gbXInputSetState_Stub;

		if (!xinput_library) xinput_library = gb_dll_load("xinput9_1_0.dll");
		if (!xinput_library) xinput_library = gb_dll_load("xinput1_3.dll");
		if (!xinput_library) {
			// TODO(bill): Proper Diagnostic
			gb_printf_err("XInput could not be loaded. Controllers will not work!\n");
		} else {
			p->xinput.get_state = cast(gbXInputGetStateProc *)gb_dll_proc_address(xinput_library, "XInputGetState");
			p->xinput.set_state = cast(gbXInputSetStateProc *)gb_dll_proc_address(xinput_library, "XInputSetState");
		}
	}

	// Init keys
	gb_zero_array(p->keys, gb_count_of(p->keys));

	p->is_initialized = true;
	return true;
}

gb_inline b32 gb_platform_init_with_software(gbPlatform *p, char const *window_title,
                                             i32 width, i32 height, u32 window_flags) {
	gbVideoMode mode;
	mode.width          = width;
	mode.height         = height;
	mode.bits_per_pixel = 32;
	return gb__platform_init(p, window_title, mode, gbRenderer_Software, window_flags);
}

gb_inline b32 gb_platform_init_with_opengl(gbPlatform *p, char const *window_title,
                                           i32 width, i32 height, u32 window_flags, i32 major, i32 minor, b32 core, b32 compatible) {
	gbVideoMode mode;
	mode.width          = width;
	mode.height         = height;
	mode.bits_per_pixel = 32;
	p->opengl.major      = major;
	p->opengl.minor      = minor;
	p->opengl.core       = cast(b16)core;
	p->opengl.compatible = cast(b16)compatible;
	return gb__platform_init(p, window_title, mode, gbRenderer_Opengl, window_flags);
}

#ifndef _XINPUT_H_
typedef struct _XINPUT_GAMEPAD {
	u16 wButtons;
	u8  bLeftTrigger;
	u8  bRightTrigger;
	u16 sThumbLX;
	u16 sThumbLY;
	u16 sThumbRX;
	u16 sThumbRY;
} XINPUT_GAMEPAD;

typedef struct _XINPUT_STATE {
	DWORD          dwPacketNumber;
	XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE;

typedef struct _XINPUT_VIBRATION {
	u16 wLeftMotorSpeed;
	u16 wRightMotorSpeed;
} XINPUT_VIBRATION;

#define XINPUT_GAMEPAD_DPAD_UP              0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN            0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT            0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT           0x00000008
#define XINPUT_GAMEPAD_START                0x00000010
#define XINPUT_GAMEPAD_BACK                 0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB           0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB          0x00000080
#define XINPUT_GAMEPAD_LEFT_SHOULDER        0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER       0x0200
#define XINPUT_GAMEPAD_A                    0x1000
#define XINPUT_GAMEPAD_B                    0x2000
#define XINPUT_GAMEPAD_X                    0x4000
#define XINPUT_GAMEPAD_Y                    0x8000
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
#endif

#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif

void gb_platform_update(gbPlatform *p) {
	isize i;

	{ // NOTE(bill): Set window state
		// TODO(bill): Should this be moved to gb__win32_window_callback ?
		RECT window_rect;
		i32 x, y, w, h;

		GetClientRect(cast(HWND)p->window_handle, &window_rect);
		x = window_rect.left;
		y = window_rect.top;
		w = window_rect.right - window_rect.left;
		h = window_rect.bottom - window_rect.top;

		if ((p->window_width != w) || (p->window_height != h)) {
			if (p->renderer_type == gbRenderer_Software)
				gb__platform_resize_dib_section(p, w, h);
		}


		p->window_x = x;
		p->window_y = y;
		p->window_width = w;
		p->window_height = h;
		GB_MASK_SET(p->window_flags, IsIconic(cast(HWND)p->window_handle) != 0, gbWindow_Minimized);

		p->window_has_focus = GetFocus() == cast(HWND)p->window_handle;
	}

	{ // NOTE(bill): Set mouse position
		POINT mouse_pos;
		DWORD win_button_id[gbMouseButton_Count] = {
			VK_LBUTTON,
			VK_MBUTTON,
			VK_RBUTTON,
			VK_XBUTTON1,
			VK_XBUTTON2,
		};

		// NOTE(bill): This needs to be GetAsyncKeyState as RAWMOUSE doesn't aways work for some odd reason
		// TODO(bill): Try and get RAWMOUSE to work for key presses
		for (i = 0; i < gbMouseButton_Count; i++)
			gb_key_state_update(p->mouse_buttons+i, GetAsyncKeyState(win_button_id[i]) < 0);

		GetCursorPos(&mouse_pos);
		ScreenToClient(cast(HWND)p->window_handle, &mouse_pos);
		{
			i32 x = mouse_pos.x;
			i32 y = p->window_height-1 - mouse_pos.y;
			p->mouse_dx = x - p->mouse_x;
			p->mouse_dy = y - p->mouse_y;
			p->mouse_x = x;
			p->mouse_y = y;
		}

		if (p->mouse_clip) {
			b32 update = false;
			i32 x = p->mouse_x;
			i32 y = p->mouse_y;
			if (p->mouse_x < 0) {
				x = 0;
				update = true;
			} else if (p->mouse_y > p->window_height-1) {
				y = p->window_height-1;
				update = true;
			}

			if (p->mouse_y < 0) {
				y = 0;
				update = true;
			} else if (p->mouse_x > p->window_width-1) {
				x = p->window_width-1;
				update = true;
			}

			if (update)
				gb_platform_set_mouse_position(p, x, y);
		}


	}


	// NOTE(bill): Set Key/Button states
	if (p->window_has_focus) {
		p->char_buffer_count = 0; // TODO(bill): Reset buffer count here or else where?

		// NOTE(bill): Need to update as the keys only get updates on events
		for (i = 0; i < gbKey_Count; i++) {
			b32 is_down = (p->keys[i] & gbKeyState_Down) != 0;
			gb_key_state_update(&p->keys[i], is_down);
		}

		p->key_modifiers.control = p->keys[gbKey_Lcontrol] | p->keys[gbKey_Rcontrol];
		p->key_modifiers.alt     = p->keys[gbKey_Lalt]     | p->keys[gbKey_Ralt];
		p->key_modifiers.shift   = p->keys[gbKey_Lshift]   | p->keys[gbKey_Rshift];

	}

	{ // NOTE(bill): Set Controller states
		isize max_controller_count = XUSER_MAX_COUNT;
		if (max_controller_count > gb_count_of(p->game_controllers))
			max_controller_count = gb_count_of(p->game_controllers);

		for (i = 0; i < max_controller_count; i++) {
			gbGameController *controller = &p->game_controllers[i];
			XINPUT_STATE controller_state = {0};
			if (p->xinput.get_state(cast(DWORD)i, &controller_state) != 0) {
				// NOTE(bill): The controller is not available
				controller->is_connected = false;
			} else {
				// NOTE(bill): This controller is plugged in
				// TODO(bill): See if ControllerState.dwPacketNumber increments too rapidly
				XINPUT_GAMEPAD *pad = &controller_state.Gamepad;

				controller->is_connected = true;

				// TODO(bill): This is a square deadzone, check XInput to verify that the deadzone is "round" and do round deadzone processing.
				controller->axes[gbControllerAxis_LeftX]  = gb__process_xinput_stick_value(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				controller->axes[gbControllerAxis_LeftY]  = gb__process_xinput_stick_value(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				controller->axes[gbControllerAxis_RightX] = gb__process_xinput_stick_value(pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				controller->axes[gbControllerAxis_RightY] = gb__process_xinput_stick_value(pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

				controller->axes[gbControllerAxis_LeftTrigger]  = cast(f32)pad->bLeftTrigger / 255.0f;
				controller->axes[gbControllerAxis_RightTrigger] = cast(f32)pad->bRightTrigger / 255.0f;


				if ((controller->axes[gbControllerAxis_LeftX] != 0.0f) ||
					(controller->axes[gbControllerAxis_LeftY] != 0.0f)) {
					controller->is_analog = true;
				}

			#define GB__PROCESS_DIGITAL_BUTTON(button_type, xinput_button) \
				gb_key_state_update(&controller->buttons[button_type], (pad->wButtons & xinput_button) == xinput_button)

				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_A,              XINPUT_GAMEPAD_A);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_B,              XINPUT_GAMEPAD_B);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_X,              XINPUT_GAMEPAD_X);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Y,              XINPUT_GAMEPAD_Y);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_LeftShoulder,  XINPUT_GAMEPAD_LEFT_SHOULDER);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Start,          XINPUT_GAMEPAD_START);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Back,           XINPUT_GAMEPAD_BACK);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Left,           XINPUT_GAMEPAD_DPAD_LEFT);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Right,          XINPUT_GAMEPAD_DPAD_RIGHT);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Down,           XINPUT_GAMEPAD_DPAD_DOWN);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_Up,             XINPUT_GAMEPAD_DPAD_UP);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_LeftThumb,     XINPUT_GAMEPAD_LEFT_THUMB);
				GB__PROCESS_DIGITAL_BUTTON(gbControllerButton_RightThumb,    XINPUT_GAMEPAD_RIGHT_THUMB);
			#undef GB__PROCESS_DIGITAL_BUTTON
			}
		}
	}

	{ // NOTE(bill): Process pending messages
		MSG message;
		for (;;) {
			BOOL is_okay = PeekMessageW(&message, 0, 0, 0, PM_REMOVE);
			if (!is_okay) break;

			switch (message.message) {
			case WM_QUIT:
				p->quit_requested = true;
				break;

			default:
				TranslateMessage(&message);
				DispatchMessageW(&message);
				break;
			}
		}
	}
}

void gb_platform_display(gbPlatform *p) {
	if (p->renderer_type == gbRenderer_Opengl) {
		SwapBuffers(cast(HDC)p->win32_dc);
	} else if (p->renderer_type == gbRenderer_Software) {
		StretchDIBits(cast(HDC)p->win32_dc,
		              0, 0, p->window_width, p->window_height,
		              0, 0, p->window_width, p->window_height,
		              p->sw_framebuffer.memory,
		              &p->sw_framebuffer.win32_bmi,
		              DIB_RGB_COLORS, SRCCOPY);
	} else {
		GB_PANIC("Invalid window rendering type");
	}

	{
		f64 prev_time = p->curr_time;
		f64 curr_time = gb_time_now();
		p->dt_for_frame = curr_time - prev_time;
		p->curr_time = curr_time;
	}
}


void gb_platform_destroy(gbPlatform *p) {
	if (p->renderer_type == gbRenderer_Opengl)
		wglDeleteContext(cast(HGLRC)p->opengl.context);
	else if (p->renderer_type == gbRenderer_Software)
		gb_vm_free(gb_virtual_memory(p->sw_framebuffer.memory, p->sw_framebuffer.memory_size));

	DestroyWindow(cast(HWND)p->window_handle);
}

void gb_platform_show_cursor(gbPlatform *p, b32 show) {
	gb_unused(p);
	ShowCursor(show);
}

void gb_platform_set_mouse_position(gbPlatform *p, i32 x, i32 y) {
	POINT point;
	point.x = cast(LONG)x;
	point.y = cast(LONG)(p->window_height-1 - y);
	ClientToScreen(cast(HWND)p->window_handle, &point);
	SetCursorPos(point.x, point.y);

	p->mouse_x = point.x;
	p->mouse_y = p->window_height-1 - point.y;
}



void gb_platform_set_controller_vibration(gbPlatform *p, isize index, f32 left_motor, f32 right_motor) {
	if (gb_is_between(index, 0, GB_MAX_GAME_CONTROLLER_COUNT-1)) {
		XINPUT_VIBRATION vibration = {0};
		left_motor  = gb_clamp01(left_motor);
		right_motor = gb_clamp01(right_motor);
		vibration.wLeftMotorSpeed  = cast(WORD)(65535 * left_motor);
		vibration.wRightMotorSpeed = cast(WORD)(65535 * right_motor);

		p->xinput.set_state(cast(DWORD)index, &vibration);
	}
}


void gb_platform_set_window_position(gbPlatform *p, i32 x, i32 y) {
	RECT rect;
	i32 width, height;

	GetClientRect(cast(HWND)p->window_handle, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;
	MoveWindow(cast(HWND)p->window_handle, x, y, width, height, false);
}

void gb_platform_set_window_title(gbPlatform *p, char const *title, ...) {
	u16 buffer[256] = {0};
	char str[512] = {0};
	va_list va;
	va_start(va, title);
	gb_snprintf_va(str, gb_size_of(str), title, va);
	va_end(va);

	if (str[0] != '\0')
		SetWindowTextW(cast(HWND)p->window_handle, cast(wchar_t const *)gb_utf8_to_ucs2(buffer, gb_size_of(buffer), str));
}

void gb_platform_toggle_fullscreen(gbPlatform *p, b32 fullscreen_desktop) {
	// NOTE(bill): From the man himself, Raymond Chen! (Modified for my need.)
	HWND handle = cast(HWND)p->window_handle;
	DWORD style = cast(DWORD)GetWindowLongW(handle, GWL_STYLE);
	WINDOWPLACEMENT placement;

	if (style & WS_OVERLAPPEDWINDOW) {
		MONITORINFO monitor_info = {gb_size_of(monitor_info)};
		if (GetWindowPlacement(handle, &placement) &&
		    GetMonitorInfoW(MonitorFromWindow(handle, 1), &monitor_info)) {
			style &= ~WS_OVERLAPPEDWINDOW;
			if (fullscreen_desktop) {
				style &= ~WS_CAPTION;
				style |= WS_POPUP;
			}
			SetWindowLongW(handle, GWL_STYLE, style);
			SetWindowPos(handle, HWND_TOP,
			             monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			             monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
			             monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
			             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			if (fullscreen_desktop)
				p->window_flags |= gbWindow_FullscreenDesktop;
			else
				p->window_flags |= gbWindow_Fullscreen;
		}
	} else {
		style &= ~WS_POPUP;
		style |= WS_OVERLAPPEDWINDOW | WS_CAPTION;
		SetWindowLongW(handle, GWL_STYLE, style);
		SetWindowPlacement(handle, &placement);
		SetWindowPos(handle, 0, 0, 0, 0, 0,
		             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		p->window_flags &= ~gbWindow_Fullscreen;
	}
}

void gb_platform_toggle_borderless(gbPlatform *p) {
	HWND handle = cast(HWND)p->window_handle;
	DWORD style = GetWindowLongW(handle, GWL_STYLE);
	b32 is_borderless = (style & WS_POPUP) != 0;

	GB_MASK_SET(style, is_borderless,  WS_OVERLAPPEDWINDOW | WS_CAPTION);
	GB_MASK_SET(style, !is_borderless, WS_POPUP);

	SetWindowLongW(handle, GWL_STYLE, style);

	GB_MASK_SET(p->window_flags, !is_borderless, gbWindow_Borderless);
}



gb_inline void gb_platform_make_opengl_context_current(gbPlatform *p) {
	if (p->renderer_type == gbRenderer_Opengl) {
		wglMakeCurrent(cast(HDC)p->win32_dc, cast(HGLRC)p->opengl.context);
	}
}

gb_inline void gb_platform_show_window(gbPlatform *p) {
	ShowWindow(cast(HWND)p->window_handle, SW_SHOW);
	p->window_flags &= ~gbWindow_Hidden;
}

gb_inline void gb_platform_hide_window(gbPlatform *p) {
	ShowWindow(cast(HWND)p->window_handle, SW_HIDE);
	p->window_flags |= gbWindow_Hidden;
}

gb_inline gbVideoMode gb_video_mode_get_desktop(void) {
	DEVMODEW win32_mode = {gb_size_of(win32_mode)};
	EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &win32_mode);
	return gb_video_mode(win32_mode.dmPelsWidth, win32_mode.dmPelsHeight, win32_mode.dmBitsPerPel);
}

isize gb_video_mode_get_fullscreen_modes(gbVideoMode *modes, isize max_mode_count) {
	DEVMODEW win32_mode = {gb_size_of(win32_mode)};
	i32 count;
	for (count = 0;
	     count < max_mode_count && EnumDisplaySettingsW(NULL, count, &win32_mode);
	     count++) {
		modes[count] = gb_video_mode(win32_mode.dmPelsWidth, win32_mode.dmPelsHeight, win32_mode.dmBitsPerPel);
	}

	gb_sort_array(modes, count, gb_video_mode_dsc_cmp);
	return count;
}



b32 gb_platform_has_clipboard_text(gbPlatform *p) {
	b32 result = false;

	if (IsClipboardFormatAvailable(1/*CF_TEXT*/) &&
	    OpenClipboard(cast(HWND)p->window_handle)) {
		HANDLE mem = GetClipboardData(1/*CF_TEXT*/);
		if (mem) {
			char *str = cast(char *)GlobalLock(mem);
			if (str && str[0] != '\0')
				result = true;
			GlobalUnlock(mem);
		} else {
			return false;
		}

		CloseClipboard();
	}

	return result;
}

// TODO(bill): Handle UTF-8
void gb_platform_set_clipboard_text(gbPlatform *p, char const *str) {
	if (OpenClipboard(cast(HWND)p->window_handle)) {
		isize i, len = gb_strlen(str)+1;

		HANDLE mem = cast(HANDLE)GlobalAlloc(0x0002/*GMEM_MOVEABLE*/, len);
		if (mem) {
			char *dst = cast(char *)GlobalLock(mem);
			if (dst) {
				for (i = 0; str[i]; i++) {
					// TODO(bill): Does this cause a buffer overflow?
					// NOTE(bill): Change \n to \r\n 'cause windows
					if (str[i] == '\n' && (i == 0 || str[i-1] != '\r')) {
						*dst++ = '\r';
					}
					*dst++ = str[i];
				}
				*dst = 0;
			}
			GlobalUnlock(mem);
		}

		EmptyClipboard();
		if (!SetClipboardData(1/*CF_TEXT*/, mem))
			return;
		CloseClipboard();
	}
}

// TODO(bill): Handle UTF-8
char *gb_platform_get_clipboard_text(gbPlatform *p, gbAllocator a) {
	char *text = NULL;

	if (IsClipboardFormatAvailable(1/*CF_TEXT*/) &&
	    OpenClipboard(cast(HWND)p->window_handle)) {
		HANDLE mem = GetClipboardData(1/*CF_TEXT*/);
		if (mem) {
			char *str = cast(char *)GlobalLock(mem);
			text = gb_alloc_str(a, str);
			GlobalUnlock(mem);
		} else {
			return NULL;
		}

		CloseClipboard();
	}

	return text;
}

#elif defined(GB_SYSTEM_OSX)

#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc.h>
#include <objc/message.h>
#include <objc/NSObjCRuntime.h>

#if __LP64__ || (TARGET_OS_EMBEDDED && !TARGET_OS_IPHONE) || TARGET_OS_WIN32 || NS_BUILD_32_LIKE_64
	#define NSIntegerEncoding  "q"
	#define NSUIntegerEncoding "L"
#else
	#define NSIntegerEncoding  "i"
	#define NSUIntegerEncoding "I"
#endif

#ifdef __OBJC__
	#import <Cocoa/Cocoa.h>
#else
	typedef CGPoint NSPoint;
	typedef CGSize  NSSize;
	typedef CGRect  NSRect;

	extern id NSApp;
	extern id const NSDefaultRunLoopMode;
#endif

#if defined(__OBJC__) && __has_feature(objc_arc)
#error TODO(bill): Cannot compile as objective-c code just yet!
#endif

// ABI is a bit different between platforms
#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#endif
#ifdef __i386__
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#else
#define abi_objc_msgSend_fpret objc_msgSend
#endif

#define objc_msgSend_id				((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void			((void (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void_id		((void (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_bool		((void (*)(id, SEL, BOOL))objc_msgSend)
#define objc_msgSend_id_char_const	((id (*)(id, SEL, char const *))objc_msgSend)

gb_internal NSUInteger gb__osx_application_should_terminate(id self, SEL _sel, id sender) {
	// NOTE(bill): Do nothing
	return 0;
}

gb_internal void gb__osx_window_will_close(id self, SEL _sel, id notification) {
	NSUInteger value = true;
	object_setInstanceVariable(self, "closed", cast(void *)value);
}

gb_internal void gb__osx_window_did_become_key(id self, SEL _sel, id notification) {
	gbPlatform *p = NULL;
	object_getInstanceVariable(self, "gbPlatform", cast(void **)&p);
	if (p) {
		// TODO(bill):
	}
}

b32 gb__platform_init(gbPlatform *p, char const *window_title, gbVideoMode mode, gbRendererType type, u32 window_flags) {
	if (p->is_initialized)
		return true;
	// Init Platform
	{ // Initial OSX State
		Class appDelegateClass;
		b32 resultAddProtoc, resultAddMethod;
		id dgAlloc, dg, menubarAlloc, menubar;
		id appMenuItemAlloc, appMenuItem;
		id appMenuAlloc, appMenu;

		#if defined(ARC_AVAILABLE)
		#error TODO(bill): This code should be compiled as C for now
		#else
		id poolAlloc = objc_msgSend_id(cast(id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc"));
		p->osx_autorelease_pool = objc_msgSend_id(poolAlloc, sel_registerName("init"));
		#endif

		objc_msgSend_id(cast(id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
		((void (*)(id, SEL, NSInteger))objc_msgSend)(NSApp, sel_registerName("setActivationPolicy:"), 0);

		appDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "AppDelegate", 0);
		resultAddProtoc = class_addProtocol(appDelegateClass, objc_getProtocol("NSApplicationDelegate"));
		assert(resultAddProtoc);
		resultAddMethod = class_addMethod(appDelegateClass, sel_registerName("applicationShouldTerminate:"), cast(IMP)gb__osx_application_should_terminate, NSUIntegerEncoding "@:@");
		assert(resultAddMethod);
		dgAlloc = objc_msgSend_id(cast(id)appDelegateClass, sel_registerName("alloc"));
		dg = objc_msgSend_id(dgAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(dg, sel_registerName("autorelease"));
		#endif

		objc_msgSend_void_id(NSApp, sel_registerName("setDelegate:"), dg);
		objc_msgSend_void(NSApp, sel_registerName("finishLaunching"));

		menubarAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenu"), sel_registerName("alloc"));
		menubar = objc_msgSend_id(menubarAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(menubar, sel_registerName("autorelease"));
		#endif

		appMenuItemAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
		appMenuItem = objc_msgSend_id(appMenuItemAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(appMenuItem, sel_registerName("autorelease"));
		#endif

		objc_msgSend_void_id(menubar, sel_registerName("addItem:"), appMenuItem);
		((id (*)(id, SEL, id))objc_msgSend)(NSApp, sel_registerName("setMainMenu:"), menubar);

		appMenuAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenu"), sel_registerName("alloc"));
		appMenu = objc_msgSend_id(appMenuAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(appMenu, sel_registerName("autorelease"));
		#endif

		{
			id processInfo = objc_msgSend_id(cast(id)objc_getClass("NSProcessInfo"), sel_registerName("processInfo"));
			id appName = objc_msgSend_id(processInfo, sel_registerName("processName"));

			id quitTitlePrefixString = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "Quit ");
			id quitTitle = ((id (*)(id, SEL, id))objc_msgSend)(quitTitlePrefixString, sel_registerName("stringByAppendingString:"), appName);

			id quitMenuItemKey = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "q");
			id quitMenuItemAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
			id quitMenuItem = ((id (*)(id, SEL, id, SEL, id))objc_msgSend)(quitMenuItemAlloc, sel_registerName("initWithTitle:action:keyEquivalent:"), quitTitle, sel_registerName("terminate:"), quitMenuItemKey);
			#ifndef ARC_AVAILABLE
			objc_msgSend_void(quitMenuItem, sel_registerName("autorelease"));
			#endif

			objc_msgSend_void_id(appMenu, sel_registerName("addItem:"), quitMenuItem);
			objc_msgSend_void_id(appMenuItem, sel_registerName("setSubmenu:"), appMenu);
		}
	}

	{ // Init Window
		NSRect rect = {{0, 0}, {cast(CGFloat)mode.width, cast(CGFloat)mode.height}};
		id windowAlloc, window, wdgAlloc, wdg, contentView, titleString;
		Class WindowDelegateClass;
		b32 resultAddProtoc, resultAddIvar, resultAddMethod;

		windowAlloc = objc_msgSend_id(cast(id)objc_getClass("NSWindow"), sel_registerName("alloc"));
		window = ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(windowAlloc, sel_registerName("initWithContentRect:styleMask:backing:defer:"), rect, 15, 2, NO);
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(window, sel_registerName("autorelease"));
		#endif

		// when we are not using ARC, than window will be added to autorelease pool
		// so if we close it by hand (pressing red button), we don't want it to be released for us
		// so it will be released by autorelease pool later
		objc_msgSend_void_bool(window, sel_registerName("setReleasedWhenClosed:"), NO);

		WindowDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "WindowDelegate", 0);
		resultAddProtoc = class_addProtocol(WindowDelegateClass, objc_getProtocol("NSWindowDelegate"));
		GB_ASSERT(resultAddProtoc);
		resultAddIvar = class_addIvar(WindowDelegateClass, "closed", gb_size_of(NSUInteger), rint(log2(gb_size_of(NSUInteger))), NSUIntegerEncoding);
		GB_ASSERT(resultAddIvar);
		resultAddIvar = class_addIvar(WindowDelegateClass, "gbPlatform", gb_size_of(void *), rint(log2(gb_size_of(void *))), "ˆv");
		GB_ASSERT(resultAddIvar);
		resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowWillClose:"), cast(IMP)gb__osx_window_will_close,  "v@:@");
		GB_ASSERT(resultAddMethod);
		resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidBecomeKey:"), cast(IMP)gb__osx_window_did_become_key,  "v@:@");
		GB_ASSERT(resultAddMethod);
		wdgAlloc = objc_msgSend_id(cast(id)WindowDelegateClass, sel_registerName("alloc"));
		wdg = objc_msgSend_id(wdgAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(wdg, sel_registerName("autorelease"));
		#endif

		objc_msgSend_void_id(window, sel_registerName("setDelegate:"), wdg);

		contentView = objc_msgSend_id(window, sel_registerName("contentView"));

		{
			NSPoint point = {20, 20};
			((void (*)(id, SEL, NSPoint))objc_msgSend)(window, sel_registerName("cascadeTopLeftFromPoint:"), point);
		}

		titleString = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), window_title);
		objc_msgSend_void_id(window, sel_registerName("setTitle:"), titleString);

		if (type == gbRenderer_Opengl) {
			// TODO(bill): Make sure this works correctly
			u32 opengl_hex_version = (p->opengl.major << 12) | (p->opengl.minor << 8);
			u32 gl_attribs[] = {
				8, 24,                  // NSOpenGLPFAColorSize, 24,
				11, 8,                  // NSOpenGLPFAAlphaSize, 8,
				5,                      // NSOpenGLPFADoubleBuffer,
				73,                     // NSOpenGLPFAAccelerated,
				//72,                   // NSOpenGLPFANoRecovery,
				//55, 1,                // NSOpenGLPFASampleBuffers, 1,
				//56, 4,                // NSOpenGLPFASamples, 4,
				99, opengl_hex_version, // NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
				0
			};

			id pixel_format_alloc, pixel_format;
			id opengl_context_alloc, opengl_context;

			pixel_format_alloc = objc_msgSend_id(cast(id)objc_getClass("NSOpenGLPixelFormat"), sel_registerName("alloc"));
			pixel_format = ((id (*)(id, SEL, const uint32_t*))objc_msgSend)(pixel_format_alloc, sel_registerName("initWithAttributes:"), gl_attribs);
			#ifndef ARC_AVAILABLE
			objc_msgSend_void(pixel_format, sel_registerName("autorelease"));
			#endif

			opengl_context_alloc = objc_msgSend_id(cast(id)objc_getClass("NSOpenGLContext"), sel_registerName("alloc"));
			opengl_context = ((id (*)(id, SEL, id, id))objc_msgSend)(opengl_context_alloc, sel_registerName("initWithFormat:shareContext:"), pixel_format, nil);
			#ifndef ARC_AVAILABLE
			objc_msgSend_void(opengl_context, sel_registerName("autorelease"));
			#endif

			objc_msgSend_void_id(opengl_context, sel_registerName("setView:"), contentView);
			objc_msgSend_void_id(window, sel_registerName("makeKeyAndOrderFront:"), window);
			objc_msgSend_void_bool(window, sel_registerName("setAcceptsMouseMovedEvents:"), YES);


			p->window_handle = cast(void *)window;
			p->opengl.context = cast(void *)opengl_context;
		} else {
			GB_PANIC("TODO(bill): Software rendering");
		}

		{
			id blackColor = objc_msgSend_id(cast(id)objc_getClass("NSColor"), sel_registerName("blackColor"));
			objc_msgSend_void_id(window, sel_registerName("setBackgroundColor:"), blackColor);
			objc_msgSend_void_bool(NSApp, sel_registerName("activateIgnoringOtherApps:"), YES);
		}
		object_setInstanceVariable(wdg, "gbPlatform", cast(void *)p);

		p->is_initialized = true;
	}

	return true;
}

// NOTE(bill): Software rendering
b32 gb_platform_init_with_software(gbPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags) {
	GB_PANIC("TODO(bill): Software rendering in not yet implemented on OS X\n");
	return gb__platform_init(p, window_title, gb_video_mode(width, height, 32), gbRenderer_Software, window_flags);
}
// NOTE(bill): OpenGL Rendering
b32 gb_platform_init_with_opengl(gbPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags,
                                 i32 major, i32 minor, b32 core, b32 compatible) {

	p->opengl.major = major;
	p->opengl.minor = minor;
	p->opengl.core  = core;
	p->opengl.compatible = compatible;
	return gb__platform_init(p, window_title, gb_video_mode(width, height, 32), gbRenderer_Opengl, window_flags);
}

// NOTE(bill): Reverse engineering can be fun!!!
gb_internal gbKeyType gb__osx_from_key_code(u16 key_code) {
	switch (key_code) {
	default: return gbKey_Unknown;
	// NOTE(bill): WHO THE FUCK DESIGNED THIS VIRTUAL KEY CODE SYSTEM?!
	// THEY ARE FUCKING IDIOTS!
	case 0x1d: return gbKey_0;
	case 0x12: return gbKey_1;
	case 0x13: return gbKey_2;
	case 0x14: return gbKey_3;
	case 0x15: return gbKey_4;
	case 0x17: return gbKey_5;
	case 0x16: return gbKey_6;
	case 0x1a: return gbKey_7;
	case 0x1c: return gbKey_8;
	case 0x19: return gbKey_9;

	case 0x00: return gbKey_A;
	case 0x0b: return gbKey_B;
	case 0x08: return gbKey_C;
	case 0x02: return gbKey_D;
	case 0x0e: return gbKey_E;
	case 0x03: return gbKey_F;
	case 0x05: return gbKey_G;
	case 0x04: return gbKey_H;
	case 0x22: return gbKey_I;
	case 0x26: return gbKey_J;
	case 0x28: return gbKey_K;
	case 0x25: return gbKey_L;
	case 0x2e: return gbKey_M;
	case 0x2d: return gbKey_N;
	case 0x1f: return gbKey_O;
	case 0x23: return gbKey_P;
	case 0x0c: return gbKey_Q;
	case 0x0f: return gbKey_R;
	case 0x01: return gbKey_S;
	case 0x11: return gbKey_T;
	case 0x20: return gbKey_U;
	case 0x09: return gbKey_V;
	case 0x0d: return gbKey_W;
	case 0x07: return gbKey_X;
	case 0x10: return gbKey_Y;
	case 0x06: return gbKey_Z;

	case 0x21: return gbKey_Lbracket;
	case 0x1e: return gbKey_Rbracket;
	case 0x29: return gbKey_Semicolon;
	case 0x2b: return gbKey_Comma;
	case 0x2f: return gbKey_Period;
	case 0x27: return gbKey_Quote;
	case 0x2c: return gbKey_Slash;
	case 0x2a: return gbKey_Backslash;
	case 0x32: return gbKey_Grave;
	case 0x18: return gbKey_Equals;
	case 0x1b: return gbKey_Minus;
	case 0x31: return gbKey_Space;

	case 0x35: return gbKey_Escape;       // Escape
	case 0x3b: return gbKey_Lcontrol;     // Left Control
	case 0x38: return gbKey_Lshift;       // Left Shift
	case 0x3a: return gbKey_Lalt;         // Left Alt
	case 0x37: return gbKey_Lsystem;      // Left OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	case 0x3e: return gbKey_Rcontrol;     // Right Control
	case 0x3c: return gbKey_Rshift;       // Right Shift
	case 0x3d: return gbKey_Ralt;         // Right Alt
	// case 0x37: return gbKey_Rsystem;      // Right OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	case 0x6e: return gbKey_Menu;         // Menu
	case 0x24: return gbKey_Return;       // Return
	case 0x33: return gbKey_Backspace;    // Backspace
	case 0x30: return gbKey_Tab;          // Tabulation
	case 0x74: return gbKey_Pageup;       // Page up
	case 0x79: return gbKey_Pagedown;     // Page down
	case 0x77: return gbKey_End;          // End
	case 0x73: return gbKey_Home;         // Home
	case 0x72: return gbKey_Insert;       // Insert
	case 0x75: return gbKey_Delete;       // Delete
	case 0x45: return gbKey_Plus;         // +
	case 0x4e: return gbKey_Subtract;     // -
	case 0x43: return gbKey_Multiply;     // *
	case 0x4b: return gbKey_Divide;       // /
	case 0x7b: return gbKey_Left;         // Left arrow
	case 0x7c: return gbKey_Right;        // Right arrow
	case 0x7e: return gbKey_Up;           // Up arrow
	case 0x7d: return gbKey_Down;         // Down arrow
	case 0x52: return gbKey_Numpad0;      // Numpad 0
	case 0x53: return gbKey_Numpad1;      // Numpad 1
	case 0x54: return gbKey_Numpad2;      // Numpad 2
	case 0x55: return gbKey_Numpad3;      // Numpad 3
	case 0x56: return gbKey_Numpad4;      // Numpad 4
	case 0x57: return gbKey_Numpad5;      // Numpad 5
	case 0x58: return gbKey_Numpad6;      // Numpad 6
	case 0x59: return gbKey_Numpad7;      // Numpad 7
	case 0x5b: return gbKey_Numpad8;      // Numpad 8
	case 0x5c: return gbKey_Numpad9;      // Numpad 9
	case 0x41: return gbKey_NumpadDot;    // Numpad .
	case 0x4c: return gbKey_NumpadEnter;  // Numpad Enter
	case 0x7a: return gbKey_F1;           // F1
	case 0x78: return gbKey_F2;           // F2
	case 0x63: return gbKey_F3;           // F3
	case 0x76: return gbKey_F4;           // F4
	case 0x60: return gbKey_F5;           // F5
	case 0x61: return gbKey_F6;           // F6
	case 0x62: return gbKey_F7;           // F7
	case 0x64: return gbKey_F8;           // F8
	case 0x65: return gbKey_F9;           // F8
	case 0x6d: return gbKey_F10;          // F10
	case 0x67: return gbKey_F11;          // F11
	case 0x6f: return gbKey_F12;          // F12
	case 0x69: return gbKey_F13;          // F13
	case 0x6b: return gbKey_F14;          // F14
	case 0x71: return gbKey_F15;          // F15
	// case : return gbKey_Pause;        // Pause // NOTE(bill): Not possible on OS X
	}
}

gb_internal void gb__osx_on_cocoa_event(gbPlatform *p, id event, id window) {
	if (!event) {
		return;
	} else if (objc_msgSend_id(window, sel_registerName("delegate"))) {
		NSUInteger event_type = ((NSUInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("type"));
		switch (event_type) {
		case 1: gb_key_state_update(&p->mouse_buttons[gbMouseButton_Left],  true);  break; // NSLeftMouseDown
		case 2: gb_key_state_update(&p->mouse_buttons[gbMouseButton_Left],  false); break; // NSLeftMouseUp
		case 3: gb_key_state_update(&p->mouse_buttons[gbMouseButton_Right], true);  break; // NSRightMouseDown
		case 4: gb_key_state_update(&p->mouse_buttons[gbMouseButton_Right], false); break; // NSRightMouseUp
		case 25: { // NSOtherMouseDown
			// TODO(bill): Test thoroughly
			NSInteger number = ((NSInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("buttonNumber"));
			if (number == 2) gb_key_state_update(&p->mouse_buttons[gbMouseButton_Middle], true);
			if (number == 3) gb_key_state_update(&p->mouse_buttons[gbMouseButton_X1],     true);
			if (number == 4) gb_key_state_update(&p->mouse_buttons[gbMouseButton_X2],     true);
		} break;
		case 26: { // NSOtherMouseUp
			NSInteger number = ((NSInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("buttonNumber"));
			if (number == 2) gb_key_state_update(&p->mouse_buttons[gbMouseButton_Middle], false);
			if (number == 3) gb_key_state_update(&p->mouse_buttons[gbMouseButton_X1],     false);
			if (number == 4) gb_key_state_update(&p->mouse_buttons[gbMouseButton_X2],     false);

		} break;

		// TODO(bill): Scroll wheel
		case 22: { // NSScrollWheel
			CGFloat dx = ((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)(event, sel_registerName("scrollingDeltaX"));
			CGFloat dy = ((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)(event, sel_registerName("scrollingDeltaY"));
			BOOL precision_scrolling = ((BOOL (*)(id, SEL))objc_msgSend)(event, sel_registerName("hasPreciseScrollingDeltas"));
			if (precision_scrolling) {
				dx *= 0.1f;
				dy *= 0.1f;
			}
			// TODO(bill): Handle sideways
			p->mouse_wheel_delta = dy;
			// p->mouse_wheel_dy = dy;
			// gb_printf("%f %f\n", dx, dy);
		} break;

		case 12: { // NSFlagsChanged
		#if 0
			// TODO(bill): Reverse engineer this properly
			NSUInteger modifiers = ((NSUInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("modifierFlags"));
			u32 upper_mask = (modifiers & 0xffff0000ul) >> 16;
			b32 shift   = (upper_mask & 0x02) != 0;
			b32 control = (upper_mask & 0x04) != 0;
			b32 alt     = (upper_mask & 0x08) != 0;
			b32 command = (upper_mask & 0x10) != 0;
		#endif

			// gb_printf("%u\n", keys.mask);
			// gb_printf("%x\n", cast(u32)modifiers);
		} break;

		case 10: { // NSKeyDown
			u16 key_code;

			id input_text = objc_msgSend_id(event, sel_registerName("characters"));
			char const *input_text_utf8 = ((char const *(*)(id, SEL))objc_msgSend)(input_text, sel_registerName("UTF8String"));
			p->char_buffer_count = gb_strnlen(input_text_utf8, gb_size_of(p->char_buffer));
			gb_memcopy(p->char_buffer, input_text_utf8, p->char_buffer_count);

			key_code = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
			gb_key_state_update(&p->keys[gb__osx_from_key_code(key_code)], true);
		} break;

		case 11: { // NSKeyUp
			u16 key_code = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
			gb_key_state_update(&p->keys[gb__osx_from_key_code(key_code)], false);
		} break;

		default: break;
		}

		objc_msgSend_void_id(NSApp, sel_registerName("sendEvent:"), event);
	}
}


void gb_platform_update(gbPlatform *p) {
	id window, key_window, content_view;
	NSRect original_frame;

	window = cast(id)p->window_handle;
	key_window = objc_msgSend_id(NSApp, sel_registerName("keyWindow"));
	p->window_has_focus = key_window == window; // TODO(bill): Is this right


	if (p->window_has_focus) {
		isize i;
		p->char_buffer_count = 0; // TODO(bill): Reset buffer count here or else where?

		// NOTE(bill): Need to update as the keys only get updates on events
		for (i = 0; i < gbKey_Count; i++) {
			b32 is_down = (p->keys[i] & gbKeyState_Down) != 0;
			gb_key_state_update(&p->keys[i], is_down);
		}

		for (i = 0; i < gbMouseButton_Count; i++) {
			b32 is_down = (p->mouse_buttons[i] & gbKeyState_Down) != 0;
			gb_key_state_update(&p->mouse_buttons[i], is_down);
		}

	}

	{ // Handle Events
		id distant_past = objc_msgSend_id(cast(id)objc_getClass("NSDate"), sel_registerName("distantPast"));
		id event = ((id (*)(id, SEL, NSUInteger, id, id, BOOL))objc_msgSend)(NSApp, sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"), NSUIntegerMax, distant_past, NSDefaultRunLoopMode, YES);
		gb__osx_on_cocoa_event(p, event, window);
	}

	if (p->window_has_focus) {
		p->key_modifiers.control = p->keys[gbKey_Lcontrol] | p->keys[gbKey_Rcontrol];
		p->key_modifiers.alt     = p->keys[gbKey_Lalt]     | p->keys[gbKey_Ralt];
		p->key_modifiers.shift   = p->keys[gbKey_Lshift]   | p->keys[gbKey_Rshift];
	}

	{ // Check if window is closed
		id wdg = objc_msgSend_id(window, sel_registerName("delegate"));
		if (!wdg) {
			p->window_is_closed = false;
		} else {
			NSUInteger value = 0;
			object_getInstanceVariable(wdg, "closed", cast(void **)&value);
			p->window_is_closed = (value != 0);
		}
	}



	content_view = objc_msgSend_id(window, sel_registerName("contentView"));
	original_frame = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(content_view, sel_registerName("frame"));

	{ // Window
		NSRect frame = original_frame;
		frame = ((NSRect (*)(id, SEL, NSRect))abi_objc_msgSend_stret)(content_view, sel_registerName("convertRectToBacking:"), frame);
		p->window_width  = frame.size.width;
		p->window_height = frame.size.height;
		frame = ((NSRect (*)(id, SEL, NSRect))abi_objc_msgSend_stret)(window, sel_registerName("convertRectToScreen:"), frame);
		p->window_x = frame.origin.x;
		p->window_y = frame.origin.y;
	}

	{ // Mouse
		NSRect frame = original_frame;
		NSPoint mouse_pos = ((NSPoint (*)(id, SEL))objc_msgSend)(window, sel_registerName("mouseLocationOutsideOfEventStream"));
		mouse_pos.x = gb_clamp(mouse_pos.x, 0, frame.size.width-1);
		mouse_pos.y = gb_clamp(mouse_pos.y, 0, frame.size.height-1);

		{
			i32 x = mouse_pos.x;
			i32 y = mouse_pos.y;
			p->mouse_dx = x - p->mouse_x;
			p->mouse_dy = y - p->mouse_y;
			p->mouse_x = x;
			p->mouse_y = y;
		}

		if (p->mouse_clip) {
			b32 update = false;
			i32 x = p->mouse_x;
			i32 y = p->mouse_y;
			if (p->mouse_x < 0) {
				x = 0;
				update = true;
			} else if (p->mouse_y > p->window_height-1) {
				y = p->window_height-1;
				update = true;
			}

			if (p->mouse_y < 0) {
				y = 0;
				update = true;
			} else if (p->mouse_x > p->window_width-1) {
				x = p->window_width-1;
				update = true;
			}

			if (update)
				gb_platform_set_mouse_position(p, x, y);
		}
	}

	{ // TODO(bill): Controllers

	}

	// TODO(bill): Is this in the correct place?
	objc_msgSend_void(NSApp, sel_registerName("updateWindows"));
	if (p->renderer_type == gbRenderer_Opengl) {
		objc_msgSend_void(cast(id)p->opengl.context, sel_registerName("update"));
		gb_platform_make_opengl_context_current(p);
	}
}

void gb_platform_display(gbPlatform *p) {
	// TODO(bill): Do more
	if (p->renderer_type == gbRenderer_Opengl) {
		gb_platform_make_opengl_context_current(p);
		objc_msgSend_void(cast(id)p->opengl.context, sel_registerName("flushBuffer"));
	} else if (p->renderer_type == gbRenderer_Software) {
		// TODO(bill):
	} else {
		GB_PANIC("Invalid window rendering type");
	}

	{
		f64 prev_time = p->curr_time;
		f64 curr_time = gb_time_now();
		p->dt_for_frame = curr_time - prev_time;
		p->curr_time = curr_time;
	}
}

void gb_platform_destroy(gbPlatform *p) {
	gb_platform_make_opengl_context_current(p);

	objc_msgSend_void(cast(id)p->window_handle, sel_registerName("close"));

	#if defined(ARC_AVAILABLE)
	// TODO(bill): autorelease pool
	#else
	objc_msgSend_void(cast(id)p->osx_autorelease_pool, sel_registerName("drain"));
	#endif
}

void gb_platform_show_cursor(gbPlatform *p, b32 show) {
	if (show ) {
		// objc_msgSend_void(class_registerName("NSCursor"), sel_registerName("unhide"));
	} else {
		// objc_msgSend_void(class_registerName("NSCursor"), sel_registerName("hide"));
	}
}

void gb_platform_set_mouse_position(gbPlatform *p, i32 x, i32 y) {
	// TODO(bill):
	CGPoint pos = {cast(CGFloat)x, cast(CGFloat)y};
	pos.x += p->window_x;
	pos.y += p->window_y;
	CGWarpMouseCursorPosition(pos);
}

void gb_platform_set_controller_vibration(gbPlatform *p, isize index, f32 left_motor, f32 right_motor) {
	// TODO(bill):
}

b32 gb_platform_has_clipboard_text(gbPlatform *p) {
	// TODO(bill):
	return false;
}

void gb_platform_set_clipboard_text(gbPlatform *p, char const *str) {
	// TODO(bill):
}

char *gb_platform_get_clipboard_text(gbPlatform *p, gbAllocator a) {
	// TODO(bill):
	return NULL;
}

void gb_platform_set_window_position(gbPlatform *p, i32 x, i32 y) {
	// TODO(bill):
}

void gb_platform_set_window_title(gbPlatform *p, char const *title, ...) {
	id title_string;
	char buf[256] = {0};
	va_list va;
	va_start(va, title);
	gb_snprintf_va(buf, gb_count_of(buf), title, va);
	va_end(va);

	title_string = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), buf);
	objc_msgSend_void_id(cast(id)p->window_handle, sel_registerName("setTitle:"), title_string);
}

void gb_platform_toggle_fullscreen(gbPlatform *p, b32 fullscreen_desktop) {
	// TODO(bill):
}

void gb_platform_toggle_borderless(gbPlatform *p) {
	// TODO(bill):
}

void gb_platform_make_opengl_context_current(gbPlatform *p) {
	objc_msgSend_void(cast(id)p->opengl.context, sel_registerName("makeCurrentContext"));
}

void gb_platform_show_window(gbPlatform *p) {
	// TODO(bill):
}

void gb_platform_hide_window(gbPlatform *p) {
	// TODO(bill):
}

i32 gb__osx_mode_bits_per_pixel(CGDisplayModeRef mode) {
	i32 bits_per_pixel = 0;
	CFStringRef pixel_encoding = CGDisplayModeCopyPixelEncoding(mode);
	if(CFStringCompare(pixel_encoding, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		bits_per_pixel = 32;
	} else if(CFStringCompare(pixel_encoding, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		bits_per_pixel = 16;
	} else if(CFStringCompare(pixel_encoding, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		bits_per_pixel = 8;
	}
    CFRelease(pixel_encoding);

	return bits_per_pixel;
}

i32 gb__osx_display_bits_per_pixel(CGDirectDisplayID display) {
	CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display);
	i32 bits_per_pixel = gb__osx_mode_bits_per_pixel(mode);
	CGDisplayModeRelease(mode);
	return bits_per_pixel;
}

gbVideoMode gb_video_mode_get_desktop(void) {
	CGDirectDisplayID display = CGMainDisplayID();
	return gb_video_mode(CGDisplayPixelsWide(display),
	                     CGDisplayPixelsHigh(display),
	                     gb__osx_display_bits_per_pixel(display));
}


isize gb_video_mode_get_fullscreen_modes(gbVideoMode *modes, isize max_mode_count) {
	CFArrayRef cg_modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), NULL);
	CFIndex i, count;
	if (cg_modes == NULL) {
		return 0;
	}

	count = gb_min(CFArrayGetCount(cg_modes), max_mode_count);
	for (i = 0; i < count; i++) {
		CGDisplayModeRef cg_mode = cast(CGDisplayModeRef)CFArrayGetValueAtIndex(cg_modes, i);
		modes[i] = gb_video_mode(CGDisplayModeGetWidth(cg_mode),
		                         CGDisplayModeGetHeight(cg_mode),
		                         gb__osx_mode_bits_per_pixel(cg_mode));
	}

	CFRelease(cg_modes);

	gb_sort_array(modes, count, gb_video_mode_dsc_cmp);
	return cast(isize)count;
}

#endif


// TODO(bill): OSX Platform Layer
// NOTE(bill): Use this as a guide so there is no need for Obj-C https://github.com/jimon/osx_app_in_plain_c

gb_inline gbVideoMode gb_video_mode(i32 width, i32 height, i32 bits_per_pixel) {
	gbVideoMode m;
	m.width = width;
	m.height = height;
	m.bits_per_pixel = bits_per_pixel;
	return m;
}

gb_inline b32 gb_video_mode_is_valid(gbVideoMode mode) {
	gb_local_persist gbVideoMode modes[256] = {0};
	gb_local_persist isize mode_count = 0;
	gb_local_persist b32 is_set = false;
	isize i;

	if (!is_set) {
		mode_count = gb_video_mode_get_fullscreen_modes(modes, gb_count_of(modes));
		is_set = true;
	}

	for (i = 0; i < mode_count; i++) {
		gb_printf("%d %d\n", modes[i].width, modes[i].height);
	}

	return gb_binary_search_array(modes, mode_count, &mode, gb_video_mode_cmp) >= 0;
}

GB_COMPARE_PROC(gb_video_mode_cmp) {
	gbVideoMode const *x = cast(gbVideoMode const *)a;
	gbVideoMode const *y = cast(gbVideoMode const *)b;

	if (x->bits_per_pixel == y->bits_per_pixel) {
		if (x->width == y->width)
			return x->height < y->height ? -1 : x->height > y->height;
		return x->width < y->width ? -1 : x->width > y->width;
	}
	return x->bits_per_pixel < y->bits_per_pixel ? -1 : +1;
}

GB_COMPARE_PROC(gb_video_mode_dsc_cmp) {
	return gb_video_mode_cmp(b, a);
}

#endif // defined(GB_PLATFORM)
