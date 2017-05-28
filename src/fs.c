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

#include "gb/fs.h"

#if defined(GB_SYSTEM_WINDOWS)
gb_internal GB_FILE_SEEK_PROC(gb__win32_file_seek) {
    LARGE_INTEGER li_offset;
    li_offset.QuadPart = offset;
    if (!SetFilePointerEx(fd.p, li_offset, &li_offset, whence)) {
      return false;
    }

    if (new_offset) *new_offset = li_offset.QuadPart;
    return true;
  }

  gb_internal GB_FILE_READ_AT_PROC(gb__win32_file_read) {
    byte32_t result = false;
    DWORD size_ = cast(DWORD)(size > INT32_MAX ? INT32_MAX : size);
    DWORD bytes_read_;
    gb__win32_file_seek(fd, offset, gbSeekWhence_Begin, NULL);
    if (ReadFile(fd.p, buffer, size_, &bytes_read_, NULL)) {
      if (bytes_read) *bytes_read = bytes_read_;
      result = true;
    }

    return result;
  }

  gb_internal GB_FILE_WRITE_AT_PROC(gb__win32_file_write) {
    DWORD size_ = cast(DWORD)(size > INT32_MAX ? INT32_MAX : size);
    DWORD bytes_written_;
    gb__win32_file_seek(fd, offset, gbSeekWhence_Begin, NULL);
    if (WriteFile(fd.p, buffer, size_, &bytes_written_, NULL)) {
      if (bytes_written) *bytes_written = bytes_written_;
      return true;
    }
    return false;
  }

  gb_internal GB_FILE_CLOSE_PROC(gb__win32_file_close) {
    CloseHandle(fd.p);
  }

  gbFileOperations const gbDefaultFileOperations = {
    gb__win32_file_read,
    gb__win32_file_write,
    gb__win32_file_seek,
    gb__win32_file_close
  };

  gb_no_inline GB_FILE_OPEN_PROC(gb__win32_file_open) {
    DWORD desired_access;
    DWORD creation_disposition;
    void *handle;
    uint16_t path[1024] = {0}; // TODO(bill): Is this really enough or should I heap allocate this if it's too large?

    switch (mode & gbFileMode_Modes) {
    case gbFileMode_Read:
      desired_access = GENERIC_READ;
      creation_disposition = OPEN_EXISTING;
      break;
    case gbFileMode_Write:
      desired_access = GENERIC_WRITE;
      creation_disposition = CREATE_ALWAYS;
      break;
    case gbFileMode_Append:
      desired_access = GENERIC_WRITE;
      creation_disposition = OPEN_ALWAYS;
      break;
    case gbFileMode_Read | gbFileMode_Rw:
      desired_access = GENERIC_READ | GENERIC_WRITE;
      creation_disposition = OPEN_EXISTING;
      break;
    case gbFileMode_Write | gbFileMode_Rw:
      desired_access = GENERIC_READ | GENERIC_WRITE;
      creation_disposition = CREATE_ALWAYS;
      break;
    case gbFileMode_Append | gbFileMode_Rw:
      desired_access = GENERIC_READ | GENERIC_WRITE;
      creation_disposition = OPEN_ALWAYS;
      break;
    default:
      GB_PANIC("Invalid file mode");
      return gbFileError_Invalid;
    }

    handle = CreateFileW(cast(wchar_t const *)gb_utf8_to_ucs2(path, gb_count_of(path), cast(uint8_t *)filename),
                         desired_access,
                         FILE_SHARE_READ|FILE_SHARE_DELETE, NULL,
                         creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle == INVALID_HANDLE_VALUE) {
      DWORD err = GetLastError();
      switch (err) {
      case ERROR_FILE_NOT_FOUND: return gbFileError_NotExists;
      case ERROR_FILE_EXISTS:    return gbFileError_Exists;
      case ERROR_ALREADY_EXISTS: return gbFileError_Exists;
      case ERROR_ACCESS_DENIED:  return gbFileError_Permission;
      }
      return gbFileError_Invalid;
    }

    if (mode & gbFileMode_Append) {
      LARGE_INTEGER offset = {0};
      if (!SetFilePointerEx(handle, offset, NULL, gbSeekWhence_End)) {
        CloseHandle(handle);
        return gbFileError_Invalid;
      }
    }

    fd->p = handle;
    *ops = gbDefaultFileOperations;
    return gbFileError_None;
  }

#else // POSIX

gb_internal GB_FILE_SEEK_PROC(gb__posix_file_seek) {
#if defined(GB_SYSTEM_OSX)
  int64_t res = lseek(fd.i, offset, whence);
#else
  int64_t res = lseek64(fd.i, offset, whence);
#endif
  if (res < 0) return false;
  if (new_offset) *new_offset = res;
  return true;
}

gb_internal GB_FILE_READ_AT_PROC(gb__posix_file_read) {
  ssize_t res = pread(fd.i, buffer, size, offset);
  if (res < 0) return false;
  if (bytes_read) *bytes_read = res;
  return true;
}

gb_internal GB_FILE_WRITE_AT_PROC(gb__posix_file_write) {
  ssize_t res;
  int64_t curr_offset = 0;
  gb__posix_file_seek(fd, 0, gbSeekWhence_Current, &curr_offset);
  if (curr_offset == offset) {
    // NOTE(bill): Writing to stdout et al. doesn't like pwrite for numerous reasons
    res = write(cast(int) fd.i, buffer, size);
  } else {
    res = pwrite(cast(int) fd.i, buffer, size, offset);
  }
  if (res < 0) return false;
  if (bytes_written) *bytes_written = res;
  return true;
}

gb_internal GB_FILE_CLOSE_PROC(gb__posix_file_close) {
  close(fd.i);
}

gbFileOperations const gbDefaultFileOperations = {
  gb__posix_file_read,
  gb__posix_file_write,
  gb__posix_file_seek,
  gb__posix_file_close
};

gb_no_inline GB_FILE_OPEN_PROC(gb__posix_file_open) {
  int32_t os_mode;
  switch (mode & gbFileMode_Modes) {
    case gbFileMode_Read:
      os_mode = O_RDONLY;
      break;
    case gbFileMode_Write:
      os_mode = O_WRONLY | O_CREAT | O_TRUNC;
      break;
    case gbFileMode_Append:
      os_mode = O_WRONLY | O_APPEND | O_CREAT;
      break;
    case gbFileMode_Read | gbFileMode_Rw:
      os_mode = O_RDWR;
      break;
    case gbFileMode_Write | gbFileMode_Rw:
      os_mode = O_RDWR | O_CREAT | O_TRUNC;
      break;
    case gbFileMode_Append | gbFileMode_Rw:
      os_mode = O_RDWR | O_APPEND | O_CREAT;
      break;
    default:
      GB_PANIC("Invalid file mode");
      return gbFileError_Invalid;
  }

  fd->i = open(filename, os_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fd->i < 0) {
    // TODO(bill): More file errors
    return gbFileError_Invalid;
  }

  *ops = gbDefaultFileOperations;
  return gbFileError_None;
}

#endif

gbFileError gb_file_new(gbFile *f, gbFileDescriptor fd, gbFileOperations ops, char const *filename) {
  gbFileError err = gbFileError_None;

  f->ops = ops;
  f->fd = fd;
  f->filename = gb_alloc_str(gb_heap_allocator(), filename);
  f->last_write_time = gb_file_last_write_time(f->filename);

  return err;
}

gbFileError gb_file_open_mode(gbFile *f, gbFileMode mode, char const *filename) {
  gbFileError err;
#if defined(GB_SYSTEM_WINDOWS)
  err = gb__win32_file_open(&f->fd, &f->ops, mode, filename);
#else
  err = gb__posix_file_open(&f->fd, &f->ops, mode, filename);
#endif
  if (err == gbFileError_None)
    return gb_file_new(f, f->fd, f->ops, filename);
  return err;
}

gbFileError gb_file_close(gbFile *f) {
  if (!f)
    return gbFileError_Invalid;

  if (f->filename) gb_free(gb_heap_allocator(), cast(char *) f->filename);

#if defined(GB_SYSTEM_WINDOWS)
  if (f->fd.p == INVALID_HANDLE_VALUE)
    return gbFileError_Invalid;
#else
  if (f->fd.i < 0)
    return gbFileError_Invalid;
#endif

  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  f->ops.close(f->fd);

  return gbFileError_None;
}

gb_inline byte32_t gb_file_read_at_check(gbFile *f, void *buffer, ssize_t size, int64_t offset, ssize_t *bytes_read) {
  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  return f->ops.read_at(f->fd, buffer, size, offset, bytes_read);
}

gb_inline byte32_t gb_file_write_at_check(gbFile *f, void const *buffer, ssize_t size, int64_t offset, ssize_t *bytes_written) {
  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  return f->ops.write_at(f->fd, buffer, size, offset, bytes_written);
}

gb_inline byte32_t gb_file_read_at(gbFile *f, void *buffer, ssize_t size, int64_t offset) {
  return gb_file_read_at_check(f, buffer, size, offset, NULL);
}

gb_inline byte32_t gb_file_write_at(gbFile *f, void const *buffer, ssize_t size, int64_t offset) {
  return gb_file_write_at_check(f, buffer, size, offset, NULL);
}

gb_inline int64_t gb_file_seek(gbFile *f, int64_t offset) {
  int64_t new_offset = 0;
  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  f->ops.seek(f->fd, offset, gbSeekWhence_Begin, &new_offset);
  return new_offset;
}

gb_inline int64_t gb_file_seek_to_end(gbFile *f) {
  int64_t new_offset = 0;
  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  f->ops.seek(f->fd, 0, gbSeekWhence_End, &new_offset);
  return new_offset;
}

// NOTE(bill): Skips a certain amount of bytes
gb_inline int64_t gb_file_skip(gbFile *f, int64_t bytes) {
  int64_t new_offset = 0;
  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  f->ops.seek(f->fd, bytes, gbSeekWhence_Current, &new_offset);
  return new_offset;
}

gb_inline int64_t gb_file_tell(gbFile *f) {
  int64_t new_offset = 0;
  if (!f->ops.read_at) f->ops = gbDefaultFileOperations;
  f->ops.seek(f->fd, 0, gbSeekWhence_Current, &new_offset);
  return new_offset;
}

gb_inline byte32_t gb_file_read(gbFile *f, void *buffer, ssize_t size) {
  return gb_file_read_at(f, buffer, size, gb_file_tell(f));
}

gb_inline byte32_t gb_file_write(gbFile *f, void const *buffer, ssize_t size) {
  return gb_file_write_at(f, buffer, size, gb_file_tell(f));
}

gbFileError gb_file_create(gbFile *f, char const *filename) {
  return gb_file_open_mode(f, gbFileMode_Write | gbFileMode_Rw, filename);
}

gbFileError gb_file_open(gbFile *f, char const *filename) {
  return gb_file_open_mode(f, gbFileMode_Read, filename);
}

char const *gb_file_name(gbFile *f) { return f->filename ? f->filename : ""; }

gb_inline byte32_t gb_file_has_changed(gbFile *f) {
  byte32_t result = false;
  gbFileTime last_write_time = gb_file_last_write_time(f->filename);
  if (f->last_write_time != last_write_time) {
    result = true;
    f->last_write_time = last_write_time;
  }
  return result;
}

// TODO(bill): Is this a bad idea?
gb_global byte32_t gb__std_file_set = false;
gb_global gbFile gb__std_files[gbFileStandard_Count] = {{0}};

#if defined(GB_SYSTEM_WINDOWS)

gb_inline gbFile *gb_file_get_standard(gbFileStandardType std) {
  if (!gb__std_file_set) {
#define GB__SET_STD_FILE(type, v) gb__std_files[type].fd.p = v; gb__std_files[type].ops = gbDefaultFileOperations
    GB__SET_STD_FILE(gbFileStandard_Input,  GetStdHandle(STD_INPUT_HANDLE));
    GB__SET_STD_FILE(gbFileStandard_Output, GetStdHandle(STD_OUTPUT_HANDLE));
    GB__SET_STD_FILE(gbFileStandard_Error,  GetStdHandle(STD_ERROR_HANDLE));
#undef GB__SET_STD_FILE
    gb__std_file_set = true;
  }
  return &gb__std_files[std];
}

gb_inline int64_t gb_file_size(gbFile *f) {
  LARGE_INTEGER size;
  GetFileSizeEx(f->fd.p, &size);
  return size.QuadPart;
}

gbFileError gb_file_truncate(gbFile *f, int64_t size) {
  gbFileError err = gbFileError_None;
  int64_t prev_offset = gb_file_tell(f);
  gb_file_seek(f, size);
  if (!SetEndOfFile(f))
    err = gbFileError_TruncationFailure;
  gb_file_seek(f, prev_offset);
  return err;
}


byte32_t gb_file_exists(char const *name) {
  WIN32_FIND_DATAW data;
  void *handle = FindFirstFileW(cast(wchar_t const *)gb_utf8_to_ucs2_buf(cast(uint8_t *)name), &data);
  byte32_t found = handle != INVALID_HANDLE_VALUE;
  if (found) FindClose(handle);
  return found;
}

#else // POSIX

gb_inline gbFile *gb_file_get_standard(gbFileStandardType std) {
  if (!gb__std_file_set) {
#define GB__SET_STD_FILE(type, v) gb__std_files[type].fd.i = v; gb__std_files[type].ops = gbDefaultFileOperations
    GB__SET_STD_FILE(gbFileStandard_Input, 0);
    GB__SET_STD_FILE(gbFileStandard_Output, 1);
    GB__SET_STD_FILE(gbFileStandard_Error, 2);
#undef GB__SET_STD_FILE
    gb__std_file_set = true;
  }
  return &gb__std_files[std];
}

gb_inline int64_t gb_file_size(gbFile *f) {
  int64_t size = 0;
  int64_t prev_offset = gb_file_tell(f);
  gb_file_seek_to_end(f);
  size = gb_file_tell(f);
  gb_file_seek(f, prev_offset);
  return size;
}

gb_inline gbFileError gb_file_truncate(gbFile *f, int64_t size) {
  gbFileError err = gbFileError_None;
  int i = ftruncate(f->fd.i, size);
  if (i != 0) err = gbFileError_TruncationFailure;
  return err;
}

gb_inline byte32_t gb_file_exists(char const *name) {
  return access(name, F_OK) != -1;
}

#endif

#if defined(GB_SYSTEM_WINDOWS)
gbFileTime gb_file_last_write_time(char const *filepath) {
  uint16_t path[1024] = {0};
  ULARGE_INTEGER li = {0};
  FILETIME last_write_time = {0};
  WIN32_FILE_ATTRIBUTE_DATA data = {0};

  if (GetFileAttributesExW(cast(wchar_t const *)gb_utf8_to_ucs2(path, gb_count_of(path), cast(uint8_t *)filepath),
                           GetFileExInfoStandard, &data))
    last_write_time = data.ftLastWriteTime;

  li.LowPart = last_write_time.dwLowDateTime;
  li.HighPart = last_write_time.dwHighDateTime;
  return cast(gbFileTime)li.QuadPart;
}


gb_inline byte32_t gb_file_copy(char const *existing_filename, char const *new_filename, byte32_t fail_if_exists) {
  uint16_t old_f[300] = {0};
  uint16_t new_f[300] = {0};

  return CopyFileW(cast(wchar_t const *)gb_utf8_to_ucs2(old_f, gb_count_of(old_f), cast(uint8_t *)existing_filename),
                   cast(wchar_t const *)gb_utf8_to_ucs2(new_f, gb_count_of(new_f), cast(uint8_t *)new_filename),
                   fail_if_exists);
}

gb_inline byte32_t gb_file_move(char const *existing_filename, char const *new_filename) {
  uint16_t old_f[300] = {0};
  uint16_t new_f[300] = {0};

  return MoveFileW(cast(wchar_t const *)gb_utf8_to_ucs2(old_f, gb_count_of(old_f), cast(uint8_t *)existing_filename),
                   cast(wchar_t const *)gb_utf8_to_ucs2(new_f, gb_count_of(new_f), cast(uint8_t *)new_filename));
}



#else

gbFileTime gb_file_last_write_time(char const *filepath) {
  time_t result = 0;
  struct stat file_stat;

  if (stat(filepath, &file_stat))
    result = file_stat.st_mtime;

  return cast(gbFileTime) result;
}

gb_inline byte32_t gb_file_copy(char const *existing_filename, char const *new_filename, byte32_t fail_if_exists) {
#if defined(GB_SYSTEM_OSX)
  return copyfile(existing_filename, new_filename, NULL, COPYFILE_DATA) == 0;
#else
  ssize_t size;
  int existing_fd = open(existing_filename, O_RDONLY, 0);
  int new_fd = open(new_filename, O_WRONLY | O_CREAT, 0666);

  struct stat stat_existing;
  fstat(existing_fd, &stat_existing);

  size = sendfile(new_fd, existing_fd, 0, stat_existing.st_size);

  close(new_fd);
  close(existing_fd);

  return size == stat_existing.st_size;
#endif
}

gb_inline byte32_t gb_file_move(char const *existing_filename, char const *new_filename) {
  if (link(existing_filename, new_filename) == 0) {
    if (unlink(existing_filename) != -1)
      return true;
  }
  return false;
}

#endif

gbFileContents gb_file_read_contents(gbAllocator a, byte32_t zero_terminate, char const *filepath) {
  gbFileContents result = {0};
  gbFile file = {0};

  result.allocator = a;

  if (gb_file_open(&file, filepath) == gbFileError_None) {
    ssize_t file_size = cast(ssize_t) gb_file_size(&file);
    if (file_size > 0) {
      result.data = gb_alloc(a, zero_terminate ? file_size + 1 : file_size);
      result.size = file_size;
      gb_file_read_at(&file, result.data, result.size, 0);
      if (zero_terminate) {
        uint8_t *str = cast(uint8_t *) result.data;
        str[file_size] = '\0';
      }
    }
    gb_file_close(&file);
  }

  return result;
}

void gb_file_free_contents(gbFileContents *fc) {
  GB_ASSERT_NOT_NULL(fc->data);
  gb_free(fc->allocator, fc->data);
  fc->data = NULL;
  fc->size = 0;
}

gb_inline byte32_t gb_path_is_absolute(char const *path) {
  byte32_t result = false;
  GB_ASSERT_NOT_NULL(path);
#if defined(GB_SYSTEM_WINDOWS)
  result == (gb_strlen(path) > 2) &&
            gb_char_is_alpha(path[0]) &&
            (path[1] == ':' && path[2] == GB_PATH_SEPARATOR);
#else
  result = (gb_strlen(path) > 0 && path[0] == GB_PATH_SEPARATOR);
#endif
  return result;
}

gb_inline byte32_t gb_path_is_relative(char const *path) { return !gb_path_is_absolute(path); }

gb_inline byte32_t gb_path_is_root(char const *path) {
  byte32_t result = false;
  GB_ASSERT_NOT_NULL(path);
#if defined(GB_SYSTEM_WINDOWS)
  result = gb_path_is_absolute(path) && (gb_strlen(path) == 3);
#else
  result = gb_path_is_absolute(path) && (gb_strlen(path) == 1);
#endif
  return result;
}

gb_inline char const *gb_path_base_name(char const *path) {
  char const *ls;
  GB_ASSERT_NOT_NULL(path);
  ls = gb_char_last_occurence(path, '/');
  return (ls == NULL) ? path : ls + 1;
}

gb_inline char const *gb_path_extension(char const *path) {
  char const *ld;
  GB_ASSERT_NOT_NULL(path);
  ld = gb_char_last_occurence(path, '.');
  return (ld == NULL) ? NULL : ld + 1;
}

#if !defined(_WINDOWS_) && defined(GB_SYSTEM_WINDOWS)
GB_DLL_IMPORT DWORD WINAPI GetFullPathNameA(char const *lpFileName, DWORD nBufferLength, char *lpBuffer, char **lpFilePart);
#endif

char *gb_path_get_full_name(gbAllocator a, char const *path) {
#if defined(GB_SYSTEM_WINDOWS)
  // TODO(bill): Make UTF-8
  char buf[300];
  ssize_t len = GetFullPathNameA(path, gb_count_of(buf), buf, NULL);
  return gb_alloc_str_len(a, buf, len+1);
#else
// TODO(bill): Make work on *nix, etc.
  char *p = realpath(path, 0);
  GB_ASSERT(p && "file does not exist");

  ssize_t len = gb_strlen(p);

// bill... gb_alloc_str_len refused to work for this...
  char *ret = gb_alloc(a, sizeof(char) * len + 1);
  gb_memmove(ret, p, len);
  ret[len] = 0;
  free(p);

  return ret;
#endif
}
