#pragma once

#include <stdlib.h>
#include <string.h>

#include "defs.h"


struct mw_string {
	char* data;
	unsigned int cap;
	unsigned int size;
};

typedef struct mw_string mw_string;

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
Initialize a string from a raw string. Can also be used to create a string from another string.
*/
static inline mw_code
mw_string_from(mw_string* str, const char* c) {

	unsigned int len = 0;
	char* curr_c = (char*) c;

	while (*curr_c != '\0') {
		len++;
		curr_c++;
	}

	mw_string_reserve(str, len);

	memcpy(str->data, c, len);
	str->size = len;

	return mw_code_ok;
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
		mw_string_reserve(str, str->cap + (str->cap >> 1));
	}

	str->data[str->size++] = c;

	return mw_code_ok;
}
