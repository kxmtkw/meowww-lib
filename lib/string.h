#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "defs.h"

#define _mw_string_growth_factor(value) value + (value >> 1)

struct mw_string {
	char* data;
	unsigned int cap;
	unsigned int size;
};

typedef struct mw_string mw_string;

static inline unsigned int
_mw_string_rawlen(const char* c) {
	unsigned int len = 0;
	char* curr_c = (char*) c;

	while (*curr_c != '\0') {
		len++;
		curr_c++;
	}

	return len;
}

/*
Create a new empty string.
*/
static inline mw_string 
mw_string_new() {
	return (mw_string) {
		NULL,
		0,
		0,
	};
}

/*
Delete a string and its contents.
*/
static inline mw_code
mw_string_delete(mw_string* str) {
	if (str->data) {
		free(str->data);
	}
	str->size = 0;
	str->cap = 0;
	return mw_code_ok;
}

/*
Reserve capacity for the string. Does nothing if new capacity < current capacity.
*/
static inline mw_code
mw_string_reserve(mw_string* str, unsigned int new_cap) {

	if (new_cap <= str->cap) {
		return mw_code_ok;
	}

	if (str->data == NULL) {
		str->data = (char*) malloc(sizeof(char) * new_cap);
	} else {
		str->data = (char*) realloc(str->data, new_cap);
	}

	str->cap = new_cap;

	return mw_code_ok;
}

/*
Resize a string. If new size < current size, it truncates the string. If new size > current size, it expands the string and fills
with null bytes. If new size is also > capacity, the new capacity will fit the new size.
*/
static inline mw_code
mw_string_resize(mw_string* str, unsigned int new_size) {

	if (new_size <= str->size) {
		str->size = new_size;
		return mw_code_ok;
	}

	if (new_size > str->cap) {
		mw_string_reserve(str, new_size);
	}

	memset(str->data + str->size, '\0', new_size - str->size);
	str->size = new_size;

	return mw_code_ok;
}

/*
Initialize a string from a raw string. Also takes the length of the raw string for O(n) copy.
*/
static inline mw_code
mw_string_froml(mw_string* str, const char* c, unsigned int len) {
	mw_string_reserve(str, len);

	memcpy(str->data, c, len);
	str->size = len;

	return mw_code_ok;
} 


/*
Initialize a string from a raw string. The process is O(n^2) because length has to be found. If length is known or you're looking for 
a more optimized process, use `mw_string_froml` instead.
*/
static inline mw_code
mw_string_from(mw_string* str, const char* c) {
	return mw_string_froml(str, c, _mw_string_rawlen(c));
}

/*
Initialize a string with another string.
*/
static inline mw_code
mw_string_froms(mw_string* str, mw_string* src) {
	return mw_string_froml(str, src->data, src->size);
}

/*
Get the current capacity of the string.
*/
static inline unsigned int 
mw_string_cap(mw_string* str) {
	return str->cap;
}

/*
Get the current size of the string.
*/
static inline unsigned int 
mw_string_size(mw_string* str) {
	return str->size;
}

/*
Get the raw data of the string. The raw string is guaranteed to end in `\0` so it is C compatible.
*/
static inline const char*
mw_string_data(mw_string* str) {
	unsigned int pos = str->size;
	mw_string_reserve(str, str->size + 1);
	str->data[pos] = '\0'; 
	// the size is actually not updated here because \0 is just for c safety. 
	return str->data;
}

/*
Get a character at `i` position. Should be used when the index is unknown. If you're iterating over the characters, it's recommended
to just access the raw data using `str.data`.
*/
static inline mw_code
mw_string_get(mw_string* str, unsigned int i, char* c) {
	if (i >= str->size) return mw_code_error;
	*c = str->data[i];
	return mw_code_ok;
}

/*
Set a character at `i` position. Should be used when the index is unknown. If you're iterating over the characters, it's recommended
to just access the raw data using `str.data`.
*/
static inline mw_code
mw_string_set(mw_string* str, unsigned int i, char c) {
	if (i >= str->size) return mw_code_error;
	str->data[i] = c;
	return mw_code_ok;
}

/*
Push a character to the end of the string. Will reserve more memory if required.
*/
static inline mw_code
mw_string_push(mw_string* str, char c) {

	if (str->size >= str->cap) {
		mw_string_reserve(str, _mw_string_growth_factor(str->cap));
	}

	str->data[str->size++] = c;

	return mw_code_ok;
}

/*
Extend the string with a raw string, reserving more memory if needed. Also takes the length of the raw string.
*/
static inline mw_code 
mw_string_extendl(mw_string* str, const char* c, unsigned int len) {

	if (str->size + len >= str->cap) {
		mw_string_reserve(str, _mw_string_growth_factor(str->cap) + len);
	}

	memcpy(str->data + str->size, c, len);
	str->size += len;

	return mw_code_ok;
}

/*
Extend the string with a raw string, reserving more memory if needed.
*/
static inline mw_code 
mw_string_extend(mw_string* str, const char* c) {
	return mw_string_extendl(str, c, _mw_string_rawlen(c));
}

/*
Extend the string with another string, reserving more memory if needed.
*/
static inline mw_code 
mw_string_extends(mw_string* str, mw_string* other) {
	return mw_string_extendl(str, other->data, other->size);
}


/*
Create a copy of the string and write it back to `return_value`. Equivalent to using `mw_string_froms`.
*/
static inline mw_code 
mw_string_copy(mw_string* str, mw_string* return_value) {
	mw_string copy = mw_string_new();
	mw_string_froms(&copy, str);
	*return_value = copy;
	return mw_code_ok;
}

/*
Check whether a string starts with specified raw string. If another string object needs to be used, use `mw_string_data`.
*/
static inline bool
mw_string_startswith(mw_string* str, const char* c) {

	unsigned int index = 0;
	char* curr = (char*)c;

	while (*curr != '\0') {

		if (index >= str->size) {
			return false;
		}

		if (*curr != str->data[index]) {
			return false;
		}

		index++;
		curr++;
	}

	return true;
}


/*
Check whether a string ends with specified raw string. If another string object needs to be used, use `mw_string_data`.
*/
static inline bool
mw_string_endswith(mw_string* str, const char* c) {

	unsigned int len = _mw_string_rawlen(c);
	unsigned int index = str->size - len;

	char* curr = (char*) c;

	while (*curr != '\0') {

		if (index >= str->size) {
			return false;
		}

		if (*curr != str->data[index]) {
			return false;
		}

		index++;
		curr++;
	}

	return true;
}