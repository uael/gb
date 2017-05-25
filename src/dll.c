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

#include "gb/dll.h"

#if defined(GB_SYSTEM_WINDOWS)

gbDllHandle gb_dll_load(char const *filepath) {
  return cast(gbDllHandle)LoadLibraryA(filepath);
}
gb_inline void      gb_dll_unload      (gbDllHandle dll)                        { FreeLibrary(cast(HMODULE)dll); }
gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) { return cast(gbDllProc)GetProcAddress(cast(HMODULE)dll, proc_name); }

#else // POSIX

gbDllHandle gb_dll_load(char const *filepath) {
  // TODO(bill): Should this be RTLD_LOCAL?
  return cast(gbDllHandle) dlopen(filepath, RTLD_LAZY | RTLD_GLOBAL);
}

gb_inline void gb_dll_unload(gbDllHandle dll) { dlclose(dll); }

gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) {
  return cast(gbDllProc) dlsym(dll, proc_name);
}

#endif
