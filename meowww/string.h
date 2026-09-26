#ifndef MEOWWW_STRING_H
#define MEOWWW_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "internal/defs.h"
#include "internal/config.h"
#include "internal/types.h"

#include "buffer.h"


/*
Reserve capacity for the string.
*/
static inline void
mw_string_reserve(mw_string* str, unsigned int new_cap) {
	mw_buffer_reserve((mw_buffer*)str, new_cap);
}

/*
Resolve the data pointer of a string.
*/
static inline char*
_mw_string_get_data(const mw_string* str) {
	return (char*)_mw_buffer_get_data((const mw_buffer*)str);
}

/*
Resize a string. Fills bytes with '\0' if the size is increased.
*/
static inline void
mw_string_resize(mw_string* str, unsigned int new_size) {
	mw_buffer_resize((mw_buffer*)str, new_size);
}

/*
Create a new string from a raw string with its length.
*/
static inline mw_string 
mw_string_new(const char* c, unsigned int len) {
	mw_buffer buf = mw_buffer_new((const mw_byte*)c, len);
	mw_string str;
	memcpy(&str, &buf, sizeof(mw_string));
	return str;
}

/*
Create a new string from a null-terminated C string.
*/
static inline mw_string 
mw_string_newc(const char* c) {
	return mw_string_new(c, strlen(c));
}

/*
Create a new string from another string.
*/
static inline mw_string 
mw_string_news(const mw_string* src) {
	return mw_string_new(_mw_string_get_data(src), src->size);
}

/*
Delete a string and its contents.
*/
static inline void
mw_string_delete(mw_string* str) {
	mw_buffer_delete((mw_buffer*)str);
}

/*
Assign a string from a raw string and its length.
*/
static inline void
mw_string_from(mw_string* str, const char* c, unsigned int len) {
	mw_buffer_from((mw_buffer*)str, (const mw_byte*)c, len);
} 

/*
Assign a string from a null-terminated C string.
*/
static inline void
mw_string_fromc(mw_string* str, const char* c) {
	mw_string_from(str, c, strlen(c));
}

/*
Assign a string from another string.
*/
static inline void
mw_string_froms(mw_string* str, const mw_string* src) {
	mw_buffer_fromb((mw_buffer*)str, (const mw_buffer*)src);
}

/*
Get current capacity of string.
*/
static inline unsigned int 
mw_string_cap(const mw_string* str) {
	return mw_buffer_cap((const mw_buffer*)str);
}

/*
Get current size of string.
*/
static inline unsigned int 
mw_string_size(const mw_string* str) {
	return mw_buffer_size((const mw_buffer*)str);
}

/*
Get the raw data of the string. The raw string is guaranteed to end in `\0` so it is C compatible.
*/
static inline const char*
mw_string_data(mw_string* str) {
	unsigned int pos = str->size;
	mw_string_reserve(str, str->size + 1); // this can cause some slowness if str.cap happens to be equal to str.size
	mw_byte* c = _mw_buffer_get_data((mw_buffer*)str);
	c[pos] = '\0';
	// the size is actually not updated here because \0 is just for c safety.
	return (char*) c;
} 

/*
Get character at index `i`.
*/
static inline char
mw_string_get(const mw_string* str, unsigned int i) {
	return (char)mw_buffer_get((const mw_buffer*)str, i);
}

/*
Set character at index `i`.
*/
static inline void
mw_string_set(mw_string* str, unsigned int i, char c) {
	mw_buffer_set((mw_buffer*)str, i, (mw_byte)c);
}

/*
Push character to end of string.
*/
static inline void
mw_string_push(mw_string* str, char c) {
	mw_buffer_push((mw_buffer*)str, (mw_byte)c);
}

/*
Pop character from end of string.
*/
static inline char
mw_string_pop(mw_string* str) {
	return (char)mw_buffer_pop((mw_buffer*)str);
}

/*
Extend string with a raw string and its length.
*/
static inline void
mw_string_extend(mw_string* str, const char* c, unsigned int len) {
	mw_buffer_extend((mw_buffer*)str, (const mw_byte*)c, len);
}

/*
Extend string with another string.
*/
static inline void
mw_string_extends(mw_string* str, const mw_string* other) {
	mw_buffer_extendb((mw_buffer*)str, (const mw_buffer*)other);
}

/*
Count occurrences of a raw sub string.
*/
static inline unsigned int
mw_string_count(const mw_string* str, const char* c, unsigned int len) {
	return mw_buffer_count((const mw_buffer*)str, (const mw_byte*)c, len);
}

/*
Count occurrences of another string.
*/
static inline unsigned int
mw_string_counts(const mw_string* str, const mw_string* other) {
	return mw_buffer_countb((const mw_buffer*)str, (const mw_buffer*)other);
}

/*
Find first index after `start` matching sub string.
*/
static inline bool
mw_string_find(const mw_string* str, const char* c, unsigned int len, unsigned int start, unsigned int* index) {
	return mw_buffer_find((const mw_buffer*)str, (const mw_byte*)c, len, start, index);
}

/*
Find first index after `start` matching another string.
*/
static inline bool
mw_string_finds(const mw_string* str, const mw_string* substr, unsigned int start, unsigned int* index) {
	return mw_buffer_findb((const mw_buffer*)str, (const mw_buffer*)substr, start, index);
}

/*
Check whether string starts with specified raw string.
*/
static inline bool
mw_string_startswith(const mw_string* str, const char* c, unsigned int len) {
	return mw_buffer_startswith((const mw_buffer*)str, (const mw_byte*)c, len);
}

/*
Check whether string ends with specified raw string.
*/
static inline bool
mw_string_endswith(const mw_string* str, const char* c, unsigned int len) {
	return mw_buffer_endswith((const mw_buffer*)str, (const mw_byte*)c, len);
}

#endif // MEOWWW_STRING_H