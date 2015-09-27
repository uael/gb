// gb_ini.h - v0.91 - public domain ini file loader library - no warranty implied; use at your own risk
// A Simple Ini File Loader Library for C and C++
//
// Version History:
//     0.91 - Add extern "C" if compiling as C++
//     0.90 - Initial Version
//
// LICENSE
//
//     This software is in the public domain. Where that dedication is not
//     recognized, you are granted a perpetual, irrevocable license to copy,
//     distribute, and modify this file as you see fit.
//
// How to use:
//
//     Do this:
//         #define GB_INI_IMPLEMENTATION
//     before you include this file in *one* C++ file to create the implementation
//
//     i.e. it should look like this:
//     #include ...
//     #include ...
//     #include ...
//     #define GB_INI_IMPLEMENTATION
//     #include "gb_ini.h"
//
//     If you prefer to use C++, you can use all the same functions in a
//     namespace instead, do this:
//         #define GB_INI_CPP
//     before you include the header file
//
//     i.e it should look like this:
//     #define GB_INI_CPP
//     #include "gb_ini.h"
//
// Examples:
//
// test.ini contents
#if 0
; This is a Comment
# This is also

name=gb_ini.h
version = 01337 # Look mum, a comment!

[license]
name = Public Domain

[author]
name = Ginger Bill
#endif
// C example
#if 0
#define GB_INI_IMPLEMENTATION
#include "gb_ini.h"

#include <stdio.h>
#include <stdlib.h>
struct Library
{
	const char* name;
	int         version;
	const char* license;
	const char* author;
};

// The below macro expands to this:
// static test_ini_handler(void* data, const char* section, const char* name, const char* value)
static GB_INI_HANDLER(test_ini_handler)
{
	struct Library* lib = (struct Library*)data;

	#define TEST(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

	if (TEST("", "name"))
		lib->name    = strdup(value);
	else if (TEST("", "version"))
		lib->version = atoi(value);
	else if (TEST("license", "name"))
		lib->license = strdup(value);
	else if (TEST("author", "name"))
		lib->author  = strdup(value);
	else
		return 0;

	#undef TEST

	return 1;
}

int main(int argc, char** argv)
{
	struct Library lib = {0};
	struct gb_Ini_Error err = gb_ini_parse("test.ini", &test_ini_handler, &lib);
	if (err.type != GB_INI_ERROR_NONE)
	{
		if (err.line_num > 0)
			printf("Line (%d): ", err.line_num);
		printf("%s\n", gb_ini_error_string(err));

		return 1;
	}

	printf("Name    : %s\n", lib.name);    // Name    : gb_init.h
	printf("Version : %d\n", lib.version); // Version : 1337
	printf("License : %s\n", lib.license); // License : Public Domain
	printf("Author  : %s\n", lib.author);  // Author  : Ginger Bill

	return 0;
}
#endif
//
// C++ example
#if 0
#define GB_INI_CPP
#define GB_INI_IMPLEMENTATION
#include "gb_ini.h"

#include <stdio.h>
#include <stdlib.h>

struct Library
{
	const char* name;
	int         version;
	const char* license;
	const char* author;
};

// The below macro expands to this:
// static test_ini_handler(void* data, const char* section, const char* name, const char* value)
static GB_INI_HANDLER(test_ini_handler)
{
	Library* lib = (Library*)data;

	#define TEST(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

	if (TEST("", "name"))
		lib->name    = strdup(value);
	else if (TEST("", "version"))
		lib->version = atoi(value);
	else if (TEST("license", "name"))
		lib->license = strdup(value);
	else if (TEST("author", "name"))
		lib->author  = strdup(value);
	else
		return 0;

	#undef TEST

	return 1;
}

int main(int argc, char** argv)
{
	Library lib = {};

	using namespace gb;

	Ini_Error err = ini_parse("test.ini", &test_ini_handler, &lib);
	if (err.type != INI_ERROR_NONE)
	{
		if (err.line_num > 0)
			printf("Line (%d): ", err.line_num);
		printf("%s\n", ini_error_string(err));

		return 1;
	}

	printf("Name    : %s\n", lib.name);    // Name    : gb_init.h
	printf("Version : %d\n", lib.version); // Version : 90
	printf("License : %s\n", lib.license); // License : Public Domain
	printf("Author  : %s\n", lib.author);  // Author  : Ginger Bill

	return 0;
}
#endif

#ifndef GB_INI_INCLUDE_GB_INI_H
#define GB_INI_INCLUDE_GB_INI_H

#ifndef GB_INI_MAX_LINE_LENGTH
#define GB_INI_MAX_LINE_LENGTH 1024
#endif

#ifndef GB_INI_MAX_SECTION_LENGTH
#define GB_INI_MAX_SECTION_LENGTH 256
#endif

#ifndef GB_INI_MAX_NAME_LENGTH
#define GB_INI_MAX_NAME_LENGTH 256
#endif

#ifndef GB_INI_CHECK_FOR_UTF8_BOM
#define GB_INI_CHECK_FOR_UTF8_BOM 1
#endif


#ifndef _MSC_VER
	#ifdef __cplusplus
	#define gb_inline inline
	#else
	#define gb_inline
	#endif
#else
#define gb_inline __forceinline
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	GB_INI_ERROR_NONE = 0,

	GB_INI_ERROR_FILE_ERROR,
	GB_INI_ERROR_MISSING_SECTION_BRACKET,
	GB_INI_ERROR_ASSIGNMENT_MISSING,
	GB_INI_ERROR_HANDLER_ERROR,

	GB_INI_ERROR_COUNT,
};

struct gb_Ini_Error
{
	int    type;
	size_t line_num;
};

#ifndef GB_INI_HANDLER_RETURN_TYPE
#define GB_INI_HANDLER_RETURN_TYPE
typedef int gb_Ini_HRT;
#endif

// This macro can be use to declare this type of function without
// needing to define all the of the parameters
#define GB_INI_HANDLER(func_name) gb_Ini_HRT func_name(void* data, const char* section, const char* name, const char* value)
typedef GB_INI_HANDLER(gb_Ini_Handler);

extern const char* GB_ERROR_STRINGS[GB_INI_ERROR_COUNT];

struct gb_Ini_Error gb_ini_parse(const char* filename, gb_Ini_Handler* handler_func, void* data);
struct gb_Ini_Error gb_ini_parse_file(FILE* file, gb_Ini_Handler* handler_func, void* data);

gb_inline const char*
gb_ini_error_string(const struct gb_Ini_Error err)
{
	return GB_ERROR_STRINGS[err.type];
}

#ifdef __cplusplus
}
#endif

#ifdef GB_INI_CPP
#if !defined(__cplusplus)
#error You need to compile as C++ for the C++ version of gb_ini.h to work
#endif

namespace gb
{
typedef gb_Ini_Error   Ini_Error;
typedef gb_Ini_Handler Ini_Handler;

// Just a copy but with the GB_ prefix stripped
enum
{
	INI_ERROR_NONE = 0,

	INI_ERROR_FILE_ERROR,
	INI_ERROR_MISSING_SECTION_BRACKET,
	INI_ERROR_ASSIGNMENT_MISSING,
	INI_ERROR_HANDLER_ERROR,

	// No need for enum count
};

Ini_Error
ini_parse(const char* filename, Ini_Handler* handler_func, void* data)
{
	return gb_ini_parse(filename, handler_func, data);
}

Ini_Error
ini_parse(FILE* file, Ini_Handler* handler_func, void* data)
{
	return gb_ini_parse_file(file, handler_func, data);
}

const char*
ini_error_string(const Ini_Error err)
{
	return GB_ERROR_STRINGS[err.type];
}

} // namespace gb
#endif // GB_INI_CPP
#endif // GB_INI_INCLUDE_GB_INI_H

#ifdef GB_INI_IMPLEMENTATION
#include <ctype.h>
#include <string.h>

const char* GB_ERROR_STRINGS[GB_INI_ERROR_COUNT] = {
	"",

	"Error in opening file",
	"Missing closing section bracket ']'",
	"Missing assignment operator '='",
	"Error in handler function",
};

static gb_inline char*
gb__left_whitespace_skip(const char* str)
{
	while (*str && isspace((unsigned char)(*str)))
		str++;
	return (char*)str;
}

static gb_inline char*
gb__right_whitespace_strip(char* str)
{
	char* end = str + strlen(str);
	while (end > str && isspace((unsigned char)(*--end)))
		*end = '\0';
	return str;
}

static gb_inline char*
gb__find_char_or_comment_in_string(const char* str, char c)
{
	int was_whitespace = 0;
	while (*str && *str != c &&
	       !(was_whitespace && *str == ';'))
	{
		was_whitespace = isspace((unsigned char)(*str));
		str++;
	}

	return (char*)str;
}

static gb_inline char*
gb__string_copy(char* dst, const char* src, size_t size)
{
	strncpy(dst, src, size);
	dst[size - 1] = '\0';
	return dst;
}


struct gb_Ini_Error
gb_ini_parse(const char* filename, gb_Ini_Handler* handler_func, void* data)
{
	struct gb_Ini_Error err = {GB_INI_ERROR_NONE, 0};

	FILE* file = fopen(filename, "r");
	if (!file)
		return err;

	err = gb_ini_parse_file(file, handler_func, data);
	fclose(file);
	return err;
}

struct gb_Ini_Error
gb_ini_parse_file(FILE* file, gb_Ini_Handler* handler_func, void* data)
{
	char line[GB_INI_MAX_LINE_LENGTH] = "";
	size_t line_num = 0;

	char section[GB_INI_MAX_SECTION_LENGTH] = "";
	char prev_name[GB_INI_MAX_NAME_LENGTH]  = "";

	char* start;
	char* end;

	struct gb_Ini_Error err = {GB_INI_ERROR_NONE, 0};

	while (fgets(line, GB_INI_MAX_LINE_LENGTH, file) != 0)
	{
		line_num++;

		start = line;

#if GB_INI_CHECK_FOR_UTF8_BOM
		// Check for UTF-8 Byte Order Mark
		if (line_num == 1 &&
		    (unsigned char)start[0] == 0xef &&
		    (unsigned char)start[1] == 0xbb &&
		    (unsigned char)start[2] == 0xbf)
		{
			start += 3;
		}
#endif

		start = gb__left_whitespace_skip(gb__right_whitespace_strip(start));

		if (start[0] == ';' || start[0] == '#')
			continue; // Allow '#' and ';' comments at start of line

		if (start[0] == '[') // [section]
		{
			end = gb__find_char_or_comment_in_string(start+1, ']');
			if (*end == ']')
			{
				*end = '\0';
				char* sect = start + 1;
				sect = gb__left_whitespace_skip(gb__right_whitespace_strip(sect));

				gb__string_copy(section, sect, sizeof(section));
				*prev_name = '\0';
			}
			else if (!err.type)
			{
				err.type = GB_INI_ERROR_MISSING_SECTION_BRACKET;
				err.line_num = line_num;
			}
		}
		else if (start[0] && start[0] != ';')
		{
			end = gb__find_char_or_comment_in_string(start, '=');
			if (*end == '=')
			{
				*end = '\0';
				char* name = gb__right_whitespace_strip(start);
				char* value = gb__left_whitespace_skip(end + 1);
				end = gb__find_char_or_comment_in_string(value, '\0');
				if (*end == ';')
					*end = '\0';
				gb__right_whitespace_strip(value);

				gb__string_copy(prev_name, name, sizeof(prev_name));
				if (!handler_func(data, section, name, value) && err.type)
				{
					err.type = GB_INI_ERROR_HANDLER_ERROR;
					err.line_num = line_num;
				}
			}
			else if (!err.type)
			{
				// No '=' found on name=value line
				err.type = GB_INI_ERROR_ASSIGNMENT_MISSING;
				err.line_num = line_num;
				continue;
			}
		}

		if (err.type)
			break;
	}

	return err;
}

#endif // GB_INI_IMPLEMENTATION
