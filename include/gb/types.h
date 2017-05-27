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

#ifndef  GB_TYPES_H__
# define GB_TYPES_H__

#include "gb/compiler.h"

#if defined(GB_COMPILER_MSVC)
#if _MSC_VER < 1300
typedef unsigned char     u8;
typedef   signed char     i8;
typedef unsigned short   u16;
typedef   signed short   i16;
typedef unsigned int     u32;
typedef   signed int     i32;
#else
typedef unsigned __int8   u8;
typedef   signed __int8   i8;
typedef unsigned __int16 u16;
typedef   signed __int16 i16;
typedef unsigned __int32 u32;
typedef   signed __int32 i32;
#endif
typedef unsigned __int64 u64;
typedef   signed __int64 i64;
#else

#include <stdint.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
#endif

GB_STATIC_ASSERT(sizeof(u8) == sizeof(i8));
GB_STATIC_ASSERT(sizeof(u16) == sizeof(i16));
GB_STATIC_ASSERT(sizeof(u32) == sizeof(i32));
GB_STATIC_ASSERT(sizeof(u64) == sizeof(i64));

GB_STATIC_ASSERT(sizeof(u8) == 1);
GB_STATIC_ASSERT(sizeof(u16) == 2);
GB_STATIC_ASSERT(sizeof(u32) == 4);
GB_STATIC_ASSERT(sizeof(u64) == 8);

typedef size_t usize;
typedef ptrdiff_t isize;

GB_STATIC_ASSERT(sizeof(usize) == sizeof(isize));

// NOTE(bill): (u)intptr is only here for semantic reasons really as this library will only support 32/64 bit OSes.
// NOTE(bill): Are there any modern OSes (not 16 bit) where intptr != isize ?
#if defined(_WIN64)
typedef signed   __int64  intptr;
  typedef unsigned __int64 uintptr;
#elif defined(_WIN32)
// NOTE(bill); To mark types changing their size, e.g. intptr
#ifndef _W64
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

  typedef _W64   signed int  intptr;
  typedef _W64 unsigned int uintptr;
#else
typedef uintptr_t uintptr;
typedef intptr_t intptr;
#endif

GB_STATIC_ASSERT(sizeof(uintptr) == sizeof(intptr));

typedef float f32;
typedef double f64;

GB_STATIC_ASSERT(sizeof(f32) == 4);
GB_STATIC_ASSERT(sizeof(f64) == 8);

typedef i32 Rune; // NOTE(bill): Unicode codepoint
#define GB_RUNE_INVALID cast(Rune)(0xfffd)
#define GB_RUNE_MAX     cast(Rune)(0x0010ffff)
#define GB_RUNE_BOM     cast(Rune)(0xfeff)
#define GB_RUNE_EOF     cast(Rune)(-1)

typedef i8 b8;
typedef i16 b16;
typedef i32 b32; // NOTE(bill): Prefer this!!!

// NOTE(bill): Get true and false
#if !defined(__cplusplus)
#if (defined(_MSC_VER) && _MSC_VER <= 1800) || (!defined(_MSC_VER) && !defined(__STDC_VERSION__))
#ifndef true
#define true  (0 == 0)
#endif
#ifndef false
#define false (0 != 0)
#endif
typedef b8 bool;
#else

#include <stdbool.h>

#endif
#endif

// NOTE(bill): These do are not prefixed with gb because the types are not.
#ifndef U8_MIN
#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define I64_MIN (-0x7fffffffffffffffll - 1)
#define I64_MAX 0x7fffffffffffffffll

#if GB_ARCH_32
#define USIZE_MIX U32_MIN
#define USIZE_MAX U32_MAX

#define ISIZE_MIX S32_MIN
#define ISIZE_MAX S32_MAX
#elif GB_ARCH_64
#define USIZE_MIX U64_MIN
#define USIZE_MAX U64_MAX

#define ISIZE_MIX I64_MIN
#define ISIZE_MAX I64_MAX
#else
#error Unknown architecture size. This library only supports 32 bit and 64 bit architectures.
#endif

#define F32_MIN 1.17549435e-38f
#define F32_MAX 3.40282347e+38f

#define F64_MIN 2.2250738585072014e-308
#define F64_MAX 1.7976931348623157e+308

#endif

#ifndef NULL
#if defined(__cplusplus)
#if __cplusplus >= 201103L
#define NULL nullptr
#else
#define NULL 0
#endif
#else
#define NULL ((void *)0)
#endif
#endif

// TODO(bill): Is this enough to get inline working?
#if !defined(__cplusplus)
#if defined(_MSC_VER) && _MSC_VER <= 1800
#define inline __inline
#elif !defined(__STDC_VERSION__)
#define inline __inline__
#else
#define inline
#endif
#endif

#if !defined(gb_restrict)
#if defined(_MSC_VER)
#define gb_restrict __restrict
#elif defined(__STDC_VERSION__)
#define gb_restrict restrict
#else
#define gb_restrict
#endif
#endif

// TODO(bill): Should force inline be a separate keyword and gb_inline be inline?
#if !defined(gb_inline)
#if defined(_MSC_VER)
#if _MSC_VER < 1300
#define gb_inline
#else
#define gb_inline __forceinline
#endif
#else
#define gb_inline __attribute__ ((__always_inline__))
#endif
#endif

#if !defined(gb_no_inline)
#if defined(_MSC_VER)
#define gb_no_inline __declspec(noinline)
#else
#define gb_no_inline __attribute__ ((noinline))
#endif
#endif

#if !defined(gb_thread_local)
#if defined(_MSC_VER) && _MSC_VER >= 1300
#define gb_thread_local __declspec(thread)
#elif defined(__GNUC__)
#define gb_thread_local __thread
#else
#define gb_thread_local thread_local
#endif
#endif


// NOTE(bill): Easy to grep
// NOTE(bill): Not needed in macros
#ifndef cast
#define cast(Type) (Type)
#endif

// NOTE(bill): Because a signed sizeof is more useful
#ifndef gb_size_of
#define gb_size_of(x) (isize)(sizeof(x))
#endif

#ifndef gb_count_of
#define gb_count_of(x) ((gb_size_of(x)/gb_size_of(0[x])) / ((isize)(!(gb_size_of(x) % gb_size_of(0[x])))))
#endif

#ifndef gb_offset_of
#define gb_offset_of(Type, element) ((isize)&(((Type *)0)->element))
#endif

#if defined(__cplusplus)
#ifndef gb_align_of
#if __cplusplus >= 201103L
#define gb_align_of(Type) (isize)alignof(Type)
#else
extern "C++" {
    // NOTE(bill): Fucking Templates!
    template <typename T> struct gbAlignment_Trick { char c; T member; };
#define gb_align_of(Type) gb_offset_of(gbAlignment_Trick<Type>, member)
}
#endif
#endif
#else
#ifndef gb_align_of
#define gb_align_of(Type) gb_offset_of(struct { char c; Type member; }, member)
#endif
#endif

// NOTE(bill): I do wish I had a type_of that was portable
#ifndef gb_swap
#define gb_swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)
#endif

// NOTE(bill): Because static means 3/4 different things in C/C++. Great design (!)
#ifndef gb_global
#define gb_global        static // Global variables
#define gb_internal      static // Internal linkage
#define gb_local_persist static // Local Persisting variables
#endif

#ifndef gb_unused
#if defined(_MSC_VER)
#define gb_unused(x) (__pragma(warning(suppress:4100))(x))
#elif defined (__GCC__)
#define gb_unused(x) __attribute__((__unused__))(x)
#else
#define gb_unused(x) ((void)(gb_size_of(x)))
#endif
#endif

#if defined(GB_COMPILER_MSVC) && !defined(_WINDOWS_)
////////////////////////////////////////////////////////////////
  //
  // Bill's Mini Windows.h
  //
  //

#define WINAPI   __stdcall
#define WINAPIV  __cdecl
#define CALLBACK __stdcall
#define MAX_PATH 260
#define CCHDEVICENAME 32
#define CCHFORMNAME   32

  typedef unsigned long DWORD;
  typedef int WINBOOL;
#ifndef XFree86Server
#ifndef __OBJC__
    typedef WINBOOL BOOL;
#else
#define BOOL WINBOOL
#endif
  typedef unsigned char BYTE;
#endif
  typedef unsigned short WORD;
  typedef float FLOAT;
  typedef int INT;
  typedef unsigned int UINT;
  typedef short SHORT;
  typedef long LONG;
  typedef long long LONGLONG;
  typedef unsigned short USHORT;
  typedef unsigned long ULONG;
  typedef unsigned long long ULONGLONG;

  typedef UINT WPARAM;
  typedef LONG LPARAM;
  typedef LONG LRESULT;
#ifndef _HRESULT_DEFINED
  typedef LONG HRESULT;
#define _HRESULT_DEFINED
#endif
#ifndef XFree86Server
  typedef WORD ATOM;
#endif /* XFree86Server */
  typedef void *HANDLE;
  typedef HANDLE HGLOBAL;
  typedef HANDLE HLOCAL;
  typedef HANDLE GLOBALHANDLE;
  typedef HANDLE LOCALHANDLE;
  typedef void *HGDIOBJ;

#define DECLARE_HANDLE(name) typedef HANDLE name
  DECLARE_HANDLE(HACCEL);
  DECLARE_HANDLE(HBITMAP);
  DECLARE_HANDLE(HBRUSH);
  DECLARE_HANDLE(HCOLORSPACE);
  DECLARE_HANDLE(HDC);
  DECLARE_HANDLE(HGLRC);
  DECLARE_HANDLE(HDESK);
  DECLARE_HANDLE(HENHMETAFILE);
  DECLARE_HANDLE(HFONT);
  DECLARE_HANDLE(HICON);
  DECLARE_HANDLE(HKEY);
  typedef HKEY *PHKEY;
  DECLARE_HANDLE(HMENU);
  DECLARE_HANDLE(HMETAFILE);
  DECLARE_HANDLE(HINSTANCE);
  typedef HINSTANCE HMODULE;
  DECLARE_HANDLE(HPALETTE);
  DECLARE_HANDLE(HPEN);
  DECLARE_HANDLE(HRGN);
  DECLARE_HANDLE(HRSRC);
  DECLARE_HANDLE(HSTR);
  DECLARE_HANDLE(HTASK);
  DECLARE_HANDLE(HWND);
  DECLARE_HANDLE(HWINSTA);
  DECLARE_HANDLE(HKL);
  DECLARE_HANDLE(HRAWINPUT);
  DECLARE_HANDLE(HMONITOR);
#undef DECLARE_HANDLE

  typedef int HFILE;
  typedef HICON HCURSOR;
  typedef DWORD COLORREF;
  typedef int (WINAPI *FARPROC)();
  typedef int (WINAPI *NEARPROC)();
  typedef int (WINAPI *PROC)();
  typedef LRESULT (CALLBACK *WNDPROC)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#if defined(_WIN64)
  typedef unsigned __int64 ULONG_PTR;
  typedef signed __int64 LONG_PTR;
#else
  typedef unsigned long ULONG_PTR;
  typedef signed long LONG_PTR;
#endif
  typedef ULONG_PTR DWORD_PTR;

  typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
  } RECT;
  typedef struct tagRECTL {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
  } RECTL;
  typedef struct tagPOINT {
    LONG x;
    LONG y;
  } POINT;
  typedef struct tagSIZE {
    LONG cx;
    LONG cy;
  } SIZE;
  typedef struct tagPOINTS {
    SHORT x;
    SHORT y;
  } POINTS;
  typedef struct _SECURITY_ATTRIBUTES {
    DWORD  nLength;
    HANDLE lpSecurityDescriptor;
    BOOL   bInheritHandle;
  } SECURITY_ATTRIBUTES;
  typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
    RelationProcessorCore,
    RelationNumaNode,
    RelationCache,
    RelationProcessorPackage,
    RelationGroup,
    RelationAll               = 0xffff
  } LOGICAL_PROCESSOR_RELATIONSHIP;
  typedef enum _PROCESSOR_CACHE_TYPE {
    CacheUnified,
    CacheInstruction,
    CacheData,
    CacheTrace
  } PROCESSOR_CACHE_TYPE;
  typedef struct _CACHE_DESCRIPTOR {
    BYTE                 Level;
    BYTE                 Associativity;
    WORD                 LineSize;
    DWORD                Size;
    PROCESSOR_CACHE_TYPE Type;
  } CACHE_DESCRIPTOR;
  typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
    ULONG_PTR                       ProcessorMask;
    LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
    union {
      struct {
        BYTE Flags;
      } ProcessorCore;
      struct {
        DWORD NodeNumber;
      } NumaNode;
      CACHE_DESCRIPTOR Cache;
      ULONGLONG        Reserved[2];
    };
  } SYSTEM_LOGICAL_PROCESSOR_INFORMATION;
  typedef struct _MEMORY_BASIC_INFORMATION {
    void *BaseAddress;
    void *AllocationBase;
    DWORD AllocationProtect;
    usize RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
  } MEMORY_BASIC_INFORMATION;
  typedef struct _SYSTEM_INFO {
    union {
      DWORD   dwOemId;
      struct {
        WORD wProcessorArchitecture;
        WORD wReserved;
      };
    };
    DWORD     dwPageSize;
    void *    lpMinimumApplicationAddress;
    void *    lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD     dwNumberOfProcessors;
    DWORD     dwProcessorType;
    DWORD     dwAllocationGranularity;
    WORD      wProcessorLevel;
    WORD      wProcessorRevision;
  } SYSTEM_INFO;
  typedef union _LARGE_INTEGER {
    struct {
      DWORD LowPart;
      LONG  HighPart;
    };
    struct {
      DWORD LowPart;
      LONG  HighPart;
    } u;
    LONGLONG QuadPart;
  } LARGE_INTEGER;
  typedef union _ULARGE_INTEGER {
    struct {
      DWORD LowPart;
      DWORD HighPart;
    };
    struct {
      DWORD LowPart;
      DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
  } ULARGE_INTEGER;

  typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
      struct {
        DWORD Offset;
        DWORD OffsetHigh;
      };
      void *Pointer;
    };
    HANDLE hEvent;
  } OVERLAPPED;
  typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
  } FILETIME;
  typedef struct _WIN32_FIND_DATAW {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwReserved0;
    DWORD    dwReserved1;
    wchar_t  cFileName[MAX_PATH];
    wchar_t  cAlternateFileName[14];
  } WIN32_FIND_DATAW;
  typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
  } WIN32_FILE_ATTRIBUTE_DATA;
  typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
  } GET_FILEEX_INFO_LEVELS;
  typedef struct tagRAWINPUTHEADER {
    DWORD  dwType;
    DWORD  dwSize;
    HANDLE hDevice;
    WPARAM wParam;
  } RAWINPUTHEADER;
  typedef struct tagRAWINPUTDEVICE {
    USHORT usUsagePage;
    USHORT usUsage;
    DWORD  dwFlags;
    HWND   hwndTarget;
  } RAWINPUTDEVICE;
  typedef struct tagRAWMOUSE {
    WORD usFlags;
    union {
      ULONG ulButtons;
      struct {
        WORD usButtonFlags;
        WORD usButtonData;
      };
    };
    ULONG ulRawButtons;
    LONG  lLastX;
    LONG  lLastY;
    ULONG ulExtraInformation;
  } RAWMOUSE;
  typedef struct tagRAWKEYBOARD {
    WORD  MakeCode;
    WORD  Flags;
    WORD  Reserved;
    WORD  VKey;
    UINT  Message;
    ULONG ExtraInformation;
  } RAWKEYBOARD;
  typedef struct tagRAWHID {
    DWORD dwSizeHid;
    DWORD dwCount;
    BYTE  bRawData[1];
  } RAWHID;
  typedef struct tagRAWINPUT {
    RAWINPUTHEADER header;
    union {
      RAWMOUSE    mouse;
      RAWKEYBOARD keyboard;
      RAWHID      hid;
    } data;
  } RAWINPUT;
  typedef struct tagWNDCLASSEXW {
    UINT           cbSize;
    UINT           style;
    WNDPROC        lpfnWndProc;
    INT            cbClsExtra;
    INT            cbWndExtra;
    HINSTANCE      hInstance;
    HICON          hIcon;
    HCURSOR        hCursor;
    HANDLE         hbrBackground;
    wchar_t const *lpszMenuName;
    wchar_t const *lpszClassName;
    HICON          hIconSm;
  } WNDCLASSEXW;
  typedef struct _POINTL {
    LONG x;
    LONG y;
  } POINTL;
  typedef struct _devicemodew {
    wchar_t dmDeviceName[CCHDEVICENAME];
    WORD    dmSpecVersion;
    WORD    dmDriverVersion;
    WORD    dmSize;
    WORD    dmDriverExtra;
    DWORD   dmFields;
    union {
      struct {
        short dmOrientation;
        short dmPaperSize;
        short dmPaperLength;
        short dmPaperWidth;
        short dmScale;
        short dmCopies;
        short dmDefaultSource;
        short dmPrintQuality;
      };
      struct {
        POINTL dmPosition;
        DWORD  dmDisplayOrientation;
        DWORD  dmDisplayFixedOutput;
      };
    };
    short   dmColor;
    short   dmDuplex;
    short   dmYResolution;
    short   dmTTOption;
    short   dmCollate;
    wchar_t dmFormName[CCHFORMNAME];
    WORD    dmLogPixels;
    DWORD   dmBitsPerPel;
    DWORD   dmPelsWidth;
    DWORD   dmPelsHeight;
    union {
      DWORD dmDisplayFlags;
      DWORD dmNup;
    };
    DWORD dmDisplayFrequency;
#if (WINVER >= 0x0400)
    DWORD dmICMMethod;
    DWORD dmICMIntent;
    DWORD dmMediaType;
    DWORD dmDitherType;
    DWORD dmReserved1;
    DWORD dmReserved2;
#if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    DWORD dmPanningWidth;
    DWORD dmPanningHeight;
#endif
#endif
  } DEVMODEW;
  typedef struct tagPIXELFORMATDESCRIPTOR {
    WORD  nSize;
    WORD  nVersion;
    DWORD dwFlags;
    BYTE  iPixelType;
    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cAccumBits;
    BYTE  cAccumRedBits;
    BYTE  cAccumGreenBits;
    BYTE  cAccumBlueBits;
    BYTE  cAccumAlphaBits;
    BYTE  cDepthBits;
    BYTE  cStencilBits;
    BYTE  cAuxBuffers;
    BYTE  iLayerType;
    BYTE  bReserved;
    DWORD dwLayerMask;
    DWORD dwVisibleMask;
    DWORD dwDamageMask;
  } PIXELFORMATDESCRIPTOR;
  typedef struct tagMSG {     // msg
    HWND   hwnd;
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
  } MSG;
  typedef struct tagWINDOWPLACEMENT {
    UINT length;
    UINT flags;
    UINT showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
  } WINDOWPLACEMENT;
  typedef struct tagMONITORINFO {
    DWORD cbSize;
    RECT  rcMonitor;
    RECT  rcWork;
    DWORD dwFlags;
  } MONITORINFO;

#define INFINITE 0xffffffffl
#define INVALID_HANDLE_VALUE ((void *)(intptr)(-1))


  typedef DWORD WINAPI THREAD_START_ROUTINE(void *parameter);

  GB_IMPORT_LINK DWORD   WINAPI GetLastError       (void);
  GB_IMPORT_LINK BOOL    WINAPI CloseHandle        (HANDLE object);
  GB_IMPORT_LINK HANDLE  WINAPI CreateSemaphoreA   (SECURITY_ATTRIBUTES *semaphore_attributes, LONG initial_count,
                                                   LONG maximum_count, char const *name);
  GB_IMPORT_LINK BOOL    WINAPI ReleaseSemaphore   (HANDLE semaphore, LONG release_count, LONG *previous_count);
  GB_IMPORT_LINK DWORD   WINAPI WaitForSingleObject(HANDLE handle, DWORD milliseconds);
  GB_IMPORT_LINK HANDLE  WINAPI CreateThread       (SECURITY_ATTRIBUTES *semaphore_attributes, usize stack_size,
                                                   THREAD_START_ROUTINE *start_address, void *parameter,
                                                   DWORD creation_flags, DWORD *thread_id);
  GB_IMPORT_LINK DWORD   WINAPI GetThreadId        (HANDLE handle);
  GB_IMPORT_LINK void    WINAPI RaiseException     (DWORD, DWORD, DWORD, ULONG_PTR const *);


  GB_IMPORT_LINK BOOL      WINAPI GetLogicalProcessorInformation(SYSTEM_LOGICAL_PROCESSOR_INFORMATION *buffer, DWORD *return_length);
  GB_IMPORT_LINK DWORD_PTR WINAPI SetThreadAffinityMask(HANDLE thread, DWORD_PTR check_mask);
  GB_IMPORT_LINK HANDLE    WINAPI GetCurrentThread(void);

#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD            0x100
#define PAGE_NOCACHE          0x200
#define PAGE_WRITECOMBINE     0x400

#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_DECOMMIT         0x4000
#define MEM_RELEASE          0x8000
#define MEM_FREE            0x10000
#define MEM_PRIVATE         0x20000
#define MEM_MAPPED          0x40000
#define MEM_RESET           0x80000
#define MEM_TOP_DOWN       0x100000
#define MEM_LARGE_PAGES  0x20000000
#define MEM_4MB_PAGES    0x80000000




  GB_IMPORT_LINK void * WINAPI VirtualAlloc (void *addr, usize size, DWORD allocation_type, DWORD protect);
  GB_IMPORT_LINK usize  WINAPI VirtualQuery (void const *address, MEMORY_BASIC_INFORMATION *buffer, usize length);
  GB_IMPORT_LINK BOOL   WINAPI VirtualFree  (void *address, usize size, DWORD free_type);
  GB_IMPORT_LINK void   WINAPI GetSystemInfo(SYSTEM_INFO *system_info);


#ifndef VK_UNKNOWN
#define VK_UNKNOWN 0
#define VK_LBUTTON  0x01
#define VK_RBUTTON  0x02
#define VK_CANCEL   0x03
#define VK_MBUTTON  0x04
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_CLEAR 0x0C
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11 // CTRL key
#define VK_MENU 0x12 // ALT key
#define VK_PAUSE 0x13 // PAUSE key
#define VK_CAPITAL 0x14 // CAPS LOCK key
#define VK_KANA 0x15 // Input Method Editor (IME) Kana mode
#define VK_HANGUL 0x15 // IME Hangul mode
#define VK_JUNJA 0x17 // IME Junja mode
#define VK_FINAL 0x18 // IME final mode
#define VK_HANJA 0x19 // IME Hanja mode
#define VK_KANJI 0x19 // IME Kanji mode
#define VK_ESCAPE 0x1B // ESC key
#define VK_CONVERT 0x1C // IME convert
#define VK_NONCONVERT 0x1D // IME nonconvert
#define VK_ACCEPT 0x1E // IME accept
#define VK_MODECHANGE 0x1F // IME mode change request
#define VK_SPACE 0x20 // SPACE key
#define VK_PRIOR 0x21 // PAGE UP key
#define VK_NEXT 0x22 // PAGE DOWN key
#define VK_END 0x23 // END key
#define VK_HOME 0x24 // HOME key
#define VK_LEFT 0x25 // LEFT ARROW key
#define VK_UP 0x26 // UP ARROW key
#define VK_RIGHT 0x27 // RIGHT ARROW key
#define VK_DOWN 0x28 // DOWN ARROW key
#define VK_SELECT 0x29 // SELECT key
#define VK_PRINT 0x2A // PRINT key
#define VK_EXECUTE 0x2B // EXECUTE key
#define VK_SNAPSHOT 0x2C // PRINT SCREEN key
#define VK_INSERT 0x2D // INS key
#define VK_DELETE 0x2E // DEL key
#define VK_HELP 0x2F // HELP key
#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39
#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A
#define VK_LWIN 0x5B // Left Windows key (Microsoft Natural keyboard)
#define VK_RWIN 0x5C // Right Windows key (Natural keyboard)
#define VK_APPS 0x5D // Applications key (Natural keyboard)
#define VK_SLEEP 0x5F // Computer Sleep key
  // Num pad keys
#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69
#define VK_MULTIPLY 0x6A
#define VK_ADD 0x6B
#define VK_SEPARATOR 0x6C
#define VK_SUBTRACT 0x6D
#define VK_DECIMAL 0x6E
#define VK_DIVIDE 0x6F
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define VK_F13 0x7C
#define VK_F14 0x7D
#define VK_F15 0x7E
#define VK_F16 0x7F
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86
#define VK_F24 0x87
#define VK_NUMLOCK 0x90
#define VK_SCROLL 0x91
#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU 0xA4
#define VK_RMENU 0xA5
#define VK_BROWSER_BACK 0xA6 // Windows 2000/XP: Browser Back key
#define VK_BROWSER_FORWARD 0xA7 // Windows 2000/XP: Browser Forward key
#define VK_BROWSER_REFRESH 0xA8 // Windows 2000/XP: Browser Refresh key
#define VK_BROWSER_STOP 0xA9 // Windows 2000/XP: Browser Stop key
#define VK_BROWSER_SEARCH 0xAA // Windows 2000/XP: Browser Search key
#define VK_BROWSER_FAVORITES 0xAB // Windows 2000/XP: Browser Favorites key
#define VK_BROWSER_HOME 0xAC // Windows 2000/XP: Browser Start and Home key
#define VK_VOLUME_MUTE 0xAD // Windows 2000/XP: Volume Mute key
#define VK_VOLUME_DOWN 0xAE // Windows 2000/XP: Volume Down key
#define VK_VOLUME_UP 0xAF // Windows 2000/XP: Volume Up key
#define VK_MEDIA_NEXT_TRACK 0xB0 // Windows 2000/XP: Next Track key
#define VK_MEDIA_PREV_TRACK 0xB1 // Windows 2000/XP: Previous Track key
#define VK_MEDIA_STOP 0xB2 // Windows 2000/XP: Stop Media key
#define VK_MEDIA_PLAY_PAUSE 0xB3 // Windows 2000/XP: Play/Pause Media key
#define VK_MEDIA_LAUNCH_MAIL 0xB4 // Windows 2000/XP: Start Mail key
#define VK_MEDIA_LAUNCH_MEDIA_SELECT 0xB5 // Windows 2000/XP: Select Media key
#define VK_MEDIA_LAUNCH_APP1 0xB6 // VK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
#define VK_MEDIA_LAUNCH_APP2 0xB7 // VK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key
#define VK_OEM_1 0xBA
#define VK_OEM_PLUS 0xBB
#define VK_OEM_COMMA 0xBC
#define VK_OEM_MINUS 0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_2 0xBF
#define VK_OEM_3 0xC0
#define VK_OEM_4 0xDB
#define VK_OEM_5 0xDC
#define VK_OEM_6 0xDD
#define VK_OEM_7 0xDE
#define VK_OEM_8 0xDF
#define VK_OEM_102 0xE2
#define VK_PROCESSKEY 0xE5
#define VK_PACKET 0xE7
#define VK_ATTN 0xF6 // Attn key
#define VK_CRSEL 0xF7 // CrSel key
#define VK_EXSEL 0xF8 // ExSel key
#define VK_EREOF 0xF9 // Erase EOF key
#define VK_PLAY 0xFA // Play key
#define VK_ZOOM 0xFB // Zoom key
#define VK_NONAME 0xFC // Reserved for future use
#define VK_PA1 0xFD // VK_PA1 (FD) PA1 key
#define VK_OEM_CLEAR 0xFE // Clear key
#endif // VK_UNKNOWN



#define GENERIC_READ             0x80000000
#define GENERIC_WRITE            0x40000000
#define GENERIC_EXECUTE          0x20000000
#define GENERIC_ALL              0x10000000
#define FILE_SHARE_READ          0x00000001
#define FILE_SHARE_WRITE         0x00000002
#define FILE_SHARE_DELETE        0x00000004
#define CREATE_NEW               1
#define CREATE_ALWAYS            2
#define OPEN_EXISTING            3
#define OPEN_ALWAYS              4
#define TRUNCATE_EXISTING        5
#define FILE_ATTRIBUTE_READONLY  0x00000001
#define FILE_ATTRIBUTE_NORMAL    0x00000080
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100
#define ERROR_FILE_NOT_FOUND     2l
#define ERROR_ACCESS_DENIED      5L
#define ERROR_NO_MORE_FILES      18l
#define ERROR_FILE_EXISTS        80l
#define ERROR_ALREADY_EXISTS     183l
#define STD_INPUT_HANDLE         ((DWORD)-10)
#define STD_OUTPUT_HANDLE        ((DWORD)-11)
#define STD_ERROR_HANDLE         ((DWORD)-12)

  GB_IMPORT_LINK BOOL   WINAPI SetFilePointerEx(HANDLE file, LARGE_INTEGER distance_to_move,
                                               LARGE_INTEGER *new_file_pointer, DWORD move_method);
  GB_IMPORT_LINK BOOL   WINAPI ReadFile        (HANDLE file, void *buffer, DWORD bytes_to_read, DWORD *bytes_read, OVERLAPPED *overlapped);
  GB_IMPORT_LINK BOOL   WINAPI WriteFile       (HANDLE file, void const *buffer, DWORD bytes_to_write, DWORD *bytes_written, OVERLAPPED *overlapped);
  GB_IMPORT_LINK HANDLE WINAPI CreateFileW     (wchar_t const *path, DWORD desired_access, DWORD share_mode,
                                               SECURITY_ATTRIBUTES *, DWORD creation_disposition,
                                               DWORD flags_and_attributes, HANDLE template_file);
  GB_IMPORT_LINK HANDLE WINAPI GetStdHandle    (DWORD std_handle);
  GB_IMPORT_LINK BOOL   WINAPI GetFileSizeEx   (HANDLE file, LARGE_INTEGER *size);
  GB_IMPORT_LINK BOOL   WINAPI SetEndOfFile    (HANDLE file);
  GB_IMPORT_LINK HANDLE WINAPI FindFirstFileW  (wchar_t const *path, WIN32_FIND_DATAW *data);
  GB_IMPORT_LINK BOOL   WINAPI FindClose       (HANDLE find_file);
  GB_IMPORT_LINK BOOL   WINAPI GetFileAttributesExW(wchar_t const *path, GET_FILEEX_INFO_LEVELS info_level_id, WIN32_FILE_ATTRIBUTE_DATA *data);
  GB_IMPORT_LINK BOOL   WINAPI CopyFileW(wchar_t const *old_f, wchar_t const *new_f, BOOL fail_if_exists);
  GB_IMPORT_LINK BOOL   WINAPI MoveFileW(wchar_t const *old_f, wchar_t const *new_f);

  GB_IMPORT_LINK HMODULE WINAPI LoadLibraryA  (char const *filename);
  GB_IMPORT_LINK BOOL    WINAPI FreeLibrary   (HMODULE module);
  GB_IMPORT_LINK FARPROC WINAPI GetProcAddress(HMODULE module, char const *name);

  GB_IMPORT_LINK BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER *frequency);
  GB_IMPORT_LINK BOOL WINAPI QueryPerformanceCounter  (LARGE_INTEGER *counter);
  GB_IMPORT_LINK void WINAPI GetSystemTimeAsFileTime  (FILETIME *system_time_as_file_time);
  GB_IMPORT_LINK void WINAPI Sleep(DWORD milliseconds);
  GB_IMPORT_LINK void WINAPI ExitProcess(UINT exit_code);

  GB_IMPORT_LINK BOOL WINAPI SetEnvironmentVariableA(char const *name, char const *value);


#define WM_NULL                   0x0000
#define WM_CREATE                 0x0001
#define WM_DESTROY                0x0002
#define WM_MOVE                   0x0003
#define WM_SIZE                   0x0005
#define WM_ACTIVATE               0x0006
#define WM_SETFOCUS               0x0007
#define WM_KILLFOCUS              0x0008
#define WM_ENABLE                 0x000A
#define WM_SETREDRAW              0x000B
#define WM_SETTEXT                0x000C
#define WM_GETTEXT                0x000D
#define WM_GETTEXTLENGTH          0x000E
#define WM_PAINT                  0x000F
#define WM_CLOSE                  0x0010
#define WM_QUERYENDSESSION        0x0011
#define WM_QUERYOPEN              0x0013
#define WM_ENDSESSION             0x0016
#define WM_QUIT                   0x0012
#define WM_ERASEBKGND             0x0014
#define WM_SYSCOLORCHANGE         0x0015
#define WM_SHOWWINDOW             0x0018
#define WM_WININICHANGE           0x001A
#define WM_SETTINGCHANGE          WM_WININICHANGE
#define WM_DEVMODECHANGE          0x001B
#define WM_ACTIVATEAPP            0x001C
#define WM_FONTCHANGE             0x001D
#define WM_TIMECHANGE             0x001E
#define WM_CANCELMODE             0x001F
#define WM_SETCURSOR              0x0020
#define WM_MOUSEACTIVATE          0x0021
#define WM_CHILDACTIVATE          0x0022
#define WM_QUEUESYNC              0x0023
#define WM_GETMINMAXINFO          0x0024
#define WM_PAINTICON              0x0026
#define WM_ICONERASEBKGND         0x0027
#define WM_NEXTDLGCTL             0x0028
#define WM_SPOOLERSTATUS          0x002A
#define WM_DRAWITEM               0x002B
#define WM_MEASUREITEM            0x002C
#define WM_DELETEITEM             0x002D
#define WM_VKEYTOITEM             0x002E
#define WM_CHARTOITEM             0x002F
#define WM_SETFONT                0x0030
#define WM_GETFONT                0x0031
#define WM_SETHOTKEY              0x0032
#define WM_GETHOTKEY              0x0033
#define WM_QUERYDRAGICON          0x0037
#define WM_COMPAREITEM            0x0039
#define WM_GETOBJECT              0x003D
#define WM_COMPACTING             0x0041
#define WM_COMMNOTIFY             0x0044  /* no longer suported */
#define WM_WINDOWPOSCHANGING      0x0046
#define WM_WINDOWPOSCHANGED       0x0047
#define WM_POWER                  0x0048
#define WM_COPYDATA               0x004A
#define WM_CANCELJOURNAL          0x004B
#define WM_NOTIFY                 0x004E
#define WM_INPUTLANGCHANGEREQUEST 0x0050
#define WM_INPUTLANGCHANGE        0x0051
#define WM_TCARD                  0x0052
#define WM_HELP                   0x0053
#define WM_USERCHANGED            0x0054
#define WM_NOTIFYFORMAT           0x0055
#define WM_CONTEXTMENU            0x007B
#define WM_STYLECHANGING          0x007C
#define WM_STYLECHANGED           0x007D
#define WM_DISPLAYCHANGE          0x007E
#define WM_GETICON                0x007F
#define WM_SETICON                0x0080
#define WM_INPUT                  0x00FF
#define WM_KEYFIRST               0x0100
#define WM_KEYDOWN                0x0100
#define WM_KEYUP                  0x0101
#define WM_CHAR                   0x0102
#define WM_DEADCHAR               0x0103
#define WM_SYSKEYDOWN             0x0104
#define WM_SYSKEYUP               0x0105
#define WM_SYSCHAR                0x0106
#define WM_SYSDEADCHAR            0x0107
#define WM_UNICHAR                0x0109
#define WM_KEYLAST                0x0109
#define WM_APP                    0x8000


#define RID_INPUT 0x10000003

#define RIM_TYPEMOUSE    0x00000000
#define RIM_TYPEKEYBOARD 0x00000001
#define RIM_TYPEHID      0x00000002

#define RI_KEY_MAKE    0x0000
#define RI_KEY_BREAK   0x0001
#define RI_KEY_E0      0x0002
#define RI_KEY_E1      0x0004
#define RI_MOUSE_WHEEL 0x0400

#define RIDEV_NOLEGACY 0x00000030

#define MAPVK_VK_TO_VSC    0
#define MAPVK_VSC_TO_VK    1
#define MAPVK_VK_TO_CHAR   2
#define MAPVK_VSC_TO_VK_EX 3

  GB_IMPORT_LINK BOOL WINAPI RegisterRawInputDevices(RAWINPUTDEVICE const *raw_input_devices, UINT num_devices, UINT size);
  GB_IMPORT_LINK UINT WINAPI GetRawInputData(HRAWINPUT raw_input, UINT ui_command, void *data, UINT *size, UINT size_header);
  GB_IMPORT_LINK UINT WINAPI MapVirtualKeyW(UINT code, UINT map_type);


#define CS_DBLCLKS 		0x0008
#define CS_VREDRAW 		0x0001
#define CS_HREDRAW 		0x0002

#define MB_OK              0x0000l
#define MB_ICONSTOP        0x0010l
#define MB_YESNO           0x0004l
#define MB_HELP            0x4000l
#define MB_ICONEXCLAMATION 0x0030l

  GB_IMPORT_LINK LRESULT WINAPI DefWindowProcW(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
  GB_IMPORT_LINK HGDIOBJ WINAPI GetStockObject(int object);
  GB_IMPORT_LINK HMODULE WINAPI GetModuleHandleW(wchar_t const *);
  GB_IMPORT_LINK ATOM    WINAPI RegisterClassExW(WNDCLASSEXW const *wcx); // u16 == ATOM
  GB_IMPORT_LINK int     WINAPI MessageBoxW(void *wnd, wchar_t const *text, wchar_t const *caption, unsigned int type);


#define DM_BITSPERPEL 0x00040000l
#define DM_PELSWIDTH  0x00080000l
#define DM_PELSHEIGHT 0x00100000l

#define CDS_FULLSCREEN 0x4
#define DISP_CHANGE_SUCCESSFUL 0
#define IDYES 6

#define WS_VISIBLE          0x10000000
#define WS_THICKFRAME       0x00040000
#define WS_MAXIMIZE         0x01000000
#define WS_MAXIMIZEBOX      0x00010000
#define WS_MINIMIZE         0x20000000
#define WS_MINIMIZEBOX      0x00020000
#define WS_POPUP            0x80000000
#define WS_OVERLAPPED	    0
#define WS_OVERLAPPEDWINDOW	0xcf0000
#define CW_USEDEFAULT       0x80000000
#define WS_BORDER           0x800000
#define WS_CAPTION          0xc00000
#define WS_SYSMENU          0x80000

#define HWND_NOTOPMOST (HWND)(-2)
#define HWND_TOPMOST   (HWND)(-1)
#define HWND_TOP       (HWND)(+0)
#define HWND_BOTTOM    (HWND)(+1)
#define SWP_NOSIZE          0x0001
#define SWP_NOMOVE          0x0002
#define SWP_NOZORDER        0x0004
#define SWP_NOREDRAW        0x0008
#define SWP_NOACTIVATE      0x0010
#define SWP_FRAMECHANGED    0x0020
#define SWP_SHOWWINDOW      0x0040
#define SWP_HIDEWINDOW      0x0080
#define SWP_NOCOPYBITS      0x0100
#define SWP_NOOWNERZORDER   0x0200
#define SWP_NOSENDCHANGING  0x0400

#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11

#define ENUM_CURRENT_SETTINGS  cast(DWORD)-1
#define ENUM_REGISTRY_SETTINGS cast(DWORD)-2

  GB_IMPORT_LINK LONG    WINAPI ChangeDisplaySettingsW(DEVMODEW *dev_mode, DWORD flags);
  GB_IMPORT_LINK BOOL    WINAPI AdjustWindowRect(RECT *rect, DWORD style, BOOL enu);
  GB_IMPORT_LINK HWND    WINAPI CreateWindowExW(DWORD ex_style, wchar_t const *class_name, wchar_t const *window_name,
                                               DWORD style, int x, int y, int width, int height, HWND wnd_parent,
                                               HMENU menu, HINSTANCE instance, void *param);
  GB_IMPORT_LINK HMODULE  WINAPI GetModuleHandleW(wchar_t const *);
  GB_IMPORT_LINK HDC             GetDC(HANDLE);
  GB_IMPORT_LINK BOOL     WINAPI GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
  GB_IMPORT_LINK BOOL            GetMonitorInfoW(HMONITOR hMonitor, MONITORINFO *lpmi);
  GB_IMPORT_LINK HMONITOR        MonitorFromWindow(HWND hwnd, DWORD dwFlags);
  GB_IMPORT_LINK LONG     WINAPI SetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong);
  GB_IMPORT_LINK BOOL     WINAPI SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
  GB_IMPORT_LINK BOOL     WINAPI SetWindowPlacement(HWND hWnd, WINDOWPLACEMENT const *lpwndpl);
  GB_IMPORT_LINK BOOL     WINAPI ShowWindow(HWND hWnd, int nCmdShow);
  GB_IMPORT_LINK LONG_PTR WINAPI GetWindowLongPtrW(HWND wnd, int index);

  GB_IMPORT_LINK BOOL           EnumDisplaySettingsW(wchar_t const *lpszDeviceName, DWORD iModeNum, DEVMODEW *lpDevMode);
  GB_IMPORT_LINK void *  WINAPI GlobalLock(HGLOBAL hMem);
  GB_IMPORT_LINK BOOL    WINAPI GlobalUnlock(HGLOBAL hMem);
  GB_IMPORT_LINK HGLOBAL WINAPI GlobalAlloc(UINT uFlags, usize dwBytes);
  GB_IMPORT_LINK HANDLE  WINAPI GetClipboardData(UINT uFormat);
  GB_IMPORT_LINK BOOL    WINAPI IsClipboardFormatAvailable(UINT format);
  GB_IMPORT_LINK BOOL    WINAPI OpenClipboard(HWND hWndNewOwner);
  GB_IMPORT_LINK BOOL    WINAPI EmptyClipboard(void);
  GB_IMPORT_LINK BOOL    WINAPI CloseClipboard(void);
  GB_IMPORT_LINK HANDLE  WINAPI SetClipboardData(UINT uFormat, HANDLE hMem);

#define PFD_TYPE_RGBA             0
#define PFD_TYPE_COLORINDEX       1
#define PFD_MAIN_PLANE            0
#define PFD_OVERLAY_PLANE         1
#define PFD_UNDERLAY_PLANE        (-1)
#define PFD_DOUBLEBUFFER          1
#define PFD_STEREO                2
#define PFD_DRAW_TO_WINDOW        4
#define PFD_DRAW_TO_BITMAP        8
#define PFD_SUPPORT_GDI           16
#define PFD_SUPPORT_OPENGL        32
#define PFD_GENERIC_FORMAT        64
#define PFD_NEED_PALETTE          128
#define PFD_NEED_SYSTEM_PALETTE   0x00000100
#define PFD_SWAP_EXCHANGE         0x00000200
#define PFD_SWAP_COPY             0x00000400
#define PFD_SWAP_LAYER_BUFFERS    0x00000800
#define PFD_GENERIC_ACCELERATED   0x00001000
#define PFD_DEPTH_DONTCARE        0x20000000
#define PFD_DOUBLEBUFFER_DONTCARE 0x40000000
#define PFD_STEREO_DONTCARE       0x80000000

#define GWLP_USERDATA -21

#define GWL_ID    -12
#define GWL_STYLE -16

  GB_IMPORT_LINK BOOL  WINAPI SetPixelFormat   (HDC hdc, int pixel_format, PIXELFORMATDESCRIPTOR const *pfd);
  GB_IMPORT_LINK int   WINAPI ChoosePixelFormat(HDC hdc, PIXELFORMATDESCRIPTOR const *pfd);
  GB_IMPORT_LINK HGLRC WINAPI wglCreateContext (HDC hdc);
  GB_IMPORT_LINK BOOL  WINAPI wglMakeCurrent   (HDC hdc, HGLRC hglrc);
  GB_IMPORT_LINK PROC  WINAPI wglGetProcAddress(char const *str);
  GB_IMPORT_LINK BOOL  WINAPI wglDeleteContext (HGLRC hglrc);

  GB_IMPORT_LINK BOOL     WINAPI SetForegroundWindow(HWND hWnd);
  GB_IMPORT_LINK HWND     WINAPI SetFocus(HWND hWnd);
  GB_IMPORT_LINK LONG_PTR WINAPI SetWindowLongPtrW(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
  GB_IMPORT_LINK BOOL     WINAPI GetClientRect(HWND hWnd, RECT *lpRect);
  GB_IMPORT_LINK BOOL     WINAPI IsIconic(HWND hWnd);
  GB_IMPORT_LINK HWND     WINAPI GetFocus(void);
  GB_IMPORT_LINK int      WINAPI ShowCursor(BOOL bShow);
  GB_IMPORT_LINK SHORT    WINAPI GetAsyncKeyState(int key);
  GB_IMPORT_LINK BOOL     WINAPI GetCursorPos(POINT *lpPoint);
  GB_IMPORT_LINK BOOL     WINAPI SetCursorPos(int x, int y);
  GB_IMPORT_LINK BOOL            ScreenToClient(HWND hWnd, POINT *lpPoint);
  GB_IMPORT_LINK BOOL            ClientToScreen(HWND hWnd, POINT *lpPoint);
  GB_IMPORT_LINK BOOL     WINAPI MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
  GB_IMPORT_LINK BOOL     WINAPI SetWindowTextW(HWND hWnd, wchar_t const *lpString);
  GB_IMPORT_LINK DWORD    WINAPI GetWindowLongW(HWND hWnd, int nIndex);




#define PM_NOREMOVE 0
#define PM_REMOVE   1

  GB_IMPORT_LINK BOOL    WINAPI PeekMessageW(MSG *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
  GB_IMPORT_LINK BOOL    WINAPI TranslateMessage(MSG const *lpMsg);
  GB_IMPORT_LINK LRESULT WINAPI DispatchMessageW(MSG const *lpMsg);

  typedef  enum
  {
    DIB_RGB_COLORS  = 0x00,
    DIB_PAL_COLORS  = 0x01,
    DIB_PAL_INDICES = 0x02
  } DIBColors;

#define SRCCOPY     (u32)0x00CC0020
#define SRCPAINT    (u32)0x00EE0086
#define SRCAND      (u32)0x008800C6
#define SRCINVERT   (u32)0x00660046
#define SRCERASE    (u32)0x00440328
#define NOTSRCCOPY  (u32)0x00330008
#define NOTSRCERASE (u32)0x001100A6
#define MERGECOPY   (u32)0x00C000CA
#define MERGEPAINT  (u32)0x00BB0226
#define PATCOPY     (u32)0x00F00021
#define PATPAINT    (u32)0x00FB0A09
#define PATINVERT   (u32)0x005A0049
#define DSTINVERT   (u32)0x00550009
#define BLACKNESS   (u32)0x00000042
#define WHITENESS   (u32)0x00FF0062

  GB_IMPORT_LINK BOOL WINAPI SwapBuffers(HDC hdc);
  GB_IMPORT_LINK BOOL WINAPI DestroyWindow(HWND hWnd);
  GB_IMPORT_LINK int         StretchDIBits(HDC hdc, int XDest, int YDest, int nDestWidth, int nDestHeight,
                                          int XSrc, int YSrc, int nSrcWidth, int nSrcHeight,
                                          void const *lpBits, /*BITMAPINFO*/void const *lpBitsInfo, UINT iUsage, DWORD dwRop);
                                          // IMPORTANT TODO(bill): FIX THIS!!!!
#endif // Bill's Mini Windows.h

//
// Defer statement
// Akin to D's SCOPE_EXIT or
// similar to Go's defer but scope-based
//
// NOTE: C++11 (and above) only!
//
#if !defined(GB_NO_DEFER) && defined(__cplusplus) && ((defined(_MSC_VER) && _MSC_VER >= 1400) || (__cplusplus >= 201103L))
extern "C++" {
  // NOTE(bill): Stupid fucking templates
  template <typename T> struct gbRemoveReference       { typedef T Type; };
  template <typename T> struct gbRemoveReference<T &>  { typedef T Type; };
  template <typename T> struct gbRemoveReference<T &&> { typedef T Type; };

  /// NOTE(bill): "Move" semantics - invented because the C++ committee are idiots (as a collective not as indiviuals (well a least some aren't))
  template <typename T> inline T &&gb_forward(typename gbRemoveReference<T>::Type &t)  { return static_cast<T &&>(t); }
  template <typename T> inline T &&gb_forward(typename gbRemoveReference<T>::Type &&t) { return static_cast<T &&>(t); }
  template <typename T> inline T &&gb_move   (T &&t)                                   { return static_cast<typename gbRemoveReference<T>::Type &&>(t); }
  template <typename F>
  struct gbprivDefer {
    F f;
    gbprivDefer(F &&f) : f(gb_forward<F>(f)) {}
    ~gbprivDefer() { f(); }
  };
  template <typename F> gbprivDefer<F> gb__defer_func(F &&f) { return gbprivDefer<F>(gb_forward<F>(f)); }

#define GB_DEFER_1(x, y) x##y
#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
#define GB_DEFER_3(x)    GB_DEFER_2(x, __COUNTER__)
#define defer(code)      auto GB_DEFER_3(_defer_) = gb__defer_func([&]()->void{code;})
}

// Example
#if 0
  gbMutex m;
  gb_mutex_init(&m);
  {
    gb_mutex_lock(&m);
    defer (gb_mutex_unlock(&m));

    ...
  }
#endif

#endif

#endif /* GB_TYPES_H__ */
