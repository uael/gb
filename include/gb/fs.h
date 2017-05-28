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

#ifndef  GB_FS_H__
# define GB_FS_H__

#include "gb/htable.h"

typedef uint32_t gbFileMode;
typedef enum gbFileModeFlag {
  gbFileMode_Read = GB_BIT(0),
  gbFileMode_Write = GB_BIT(1),
  gbFileMode_Append = GB_BIT(2),
  gbFileMode_Rw = GB_BIT(3),

  gbFileMode_Modes = gbFileMode_Read | gbFileMode_Write | gbFileMode_Append | gbFileMode_Rw,
} gbFileModeFlag;

// NOTE(bill): Only used internally and for the file operations
typedef enum gbSeekWhenceType {
  gbSeekWhence_Begin = 0,
  gbSeekWhence_Current = 1,
  gbSeekWhence_End = 2,
} gbSeekWhenceType;

typedef enum gbFileError {
  gbFileError_None,
  gbFileError_Invalid,
  gbFileError_Exists,
  gbFileError_NotExists,
  gbFileError_Permission,
  gbFileError_TruncationFailure,
} gbFileError;

typedef union gbFileDescriptor {
  void *p;
  intptr_t i;
  uintptr_t u;
} gbFileDescriptor;

typedef struct gbFileOperations gbFileOperations;

#define GB_FILE_OPEN_PROC(name)     gbFileError name(gbFileDescriptor *fd, gbFileOperations *ops, gbFileMode mode, char const *filename)
#define GB_FILE_READ_AT_PROC(name)  byte32_t         name(gbFileDescriptor fd, void *buffer, ssize_t size, int64_t offset, ssize_t *bytes_read)
#define GB_FILE_WRITE_AT_PROC(name) byte32_t         name(gbFileDescriptor fd, void const *buffer, ssize_t size, int64_t offset, ssize_t *bytes_written)
#define GB_FILE_SEEK_PROC(name)     byte32_t         name(gbFileDescriptor fd, int64_t offset, gbSeekWhenceType whence, int64_t *new_offset)
#define GB_FILE_CLOSE_PROC(name)    void        name(gbFileDescriptor fd)

typedef GB_FILE_OPEN_PROC(gbFileOpenProc);

typedef GB_FILE_READ_AT_PROC(gbFileReadProc);

typedef GB_FILE_WRITE_AT_PROC(gbFileWriteProc);

typedef GB_FILE_SEEK_PROC(gbFileSeekProc);

typedef GB_FILE_CLOSE_PROC(gbFileCloseProc);

struct gbFileOperations {
  gbFileReadProc *read_at;
  gbFileWriteProc *write_at;
  gbFileSeekProc *seek;
  gbFileCloseProc *close;
};

extern gbFileOperations const gbDefaultFileOperations;


// typedef struct gbDirInfo {
// 	uint8_t *buf;
// 	ssize_t buf_count;
// 	ssize_t buf_pos;
// } gbDirInfo;

typedef uint64_t gbFileTime;

typedef struct gbFile {
  gbFileOperations ops;
  gbFileDescriptor fd;
  char const *filename;
  gbFileTime last_write_time;
  // gbDirInfo *   dir_info; // TODO(bill): Get directory info
} gbFile;

// TODO(bill): gbAsyncFile

typedef enum gbFileStandardType {
  gbFileStandard_Input,
  gbFileStandard_Output,
  gbFileStandard_Error,

  gbFileStandard_Count,
} gbFileStandardType;

GB_DEF gbFile *gb_file_get_standard(gbFileStandardType std);

GB_DEF gbFileError gb_file_create(gbFile *file, char const *filename);

GB_DEF gbFileError gb_file_open(gbFile *file, char const *filename);

GB_DEF gbFileError gb_file_open_mode(gbFile *file, gbFileMode mode, char const *filename);

GB_DEF gbFileError gb_file_new(gbFile *file, gbFileDescriptor fd, gbFileOperations ops, char const *filename);

GB_DEF byte32_t gb_file_read_at_check(gbFile *file, void *buffer, ssize_t size, int64_t offset, ssize_t *bytes_read);

GB_DEF byte32_t gb_file_write_at_check(gbFile *file, void const *buffer, ssize_t size, int64_t offset, ssize_t *bytes_written);

GB_DEF byte32_t gb_file_read_at(gbFile *file, void *buffer, ssize_t size, int64_t offset);

GB_DEF byte32_t gb_file_write_at(gbFile *file, void const *buffer, ssize_t size, int64_t offset);

GB_DEF int64_t gb_file_seek(gbFile *file, int64_t offset);

GB_DEF int64_t gb_file_seek_to_end(gbFile *file);

GB_DEF int64_t gb_file_skip(gbFile *file, int64_t bytes); // NOTE(bill): Skips a certain amount of bytes
GB_DEF int64_t gb_file_tell(gbFile *file);

GB_DEF gbFileError gb_file_close(gbFile *file);

GB_DEF byte32_t gb_file_read(gbFile *file, void *buffer, ssize_t size);

GB_DEF byte32_t gb_file_write(gbFile *file, void const *buffer, ssize_t size);

GB_DEF int64_t gb_file_size(gbFile *file);

GB_DEF char const *gb_file_name(gbFile *file);

GB_DEF gbFileError gb_file_truncate(gbFile *file, int64_t size);

GB_DEF byte32_t gb_file_has_changed(gbFile *file); // NOTE(bill): Changed since lasted checked
// TODO(bill):
// gbFileError gb_file_temp(gbFile *file);
//

typedef struct gbFileContents {
  gb_allocator_t allocator;
  void *data;
  ssize_t size;
} gbFileContents;

GB_DEF gbFileContents gb_file_read_contents(gb_allocator_t a, byte32_t zero_terminate, char const *filepath);

GB_DEF void gb_file_free_contents(gbFileContents *fc);


// TODO(bill): Should these have different na,es as they do not take in a gbFile * ???
GB_DEF byte32_t gb_file_exists(char const *filepath);

GB_DEF gbFileTime gb_file_last_write_time(char const *filepath);

GB_DEF byte32_t gb_file_copy(char const *existing_filename, char const *new_filename, byte32_t fail_if_exists);

GB_DEF byte32_t gb_file_move(char const *existing_filename, char const *new_filename);

#ifndef GB_PATH_SEPARATOR
#if defined(GB_SYSTEM_WINDOWS)
#define GB_PATH_SEPARATOR '\\'
#else
#define GB_PATH_SEPARATOR '/'
#endif
#endif

GB_DEF byte32_t gb_path_is_absolute(char const *path);

GB_DEF byte32_t gb_path_is_relative(char const *path);

GB_DEF byte32_t gb_path_is_root(char const *path);

GB_DEF char const *gb_path_base_name(char const *path);

GB_DEF char const *gb_path_extension(char const *path);

GB_DEF char *gb_path_get_full_name(gb_allocator_t a, char const *path);

#endif /* GB_FS_H__ */
