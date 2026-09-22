#ifndef MEOWWW_STRING_H
#define MEOWWW_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "defs.h"

#define _MW_STRING_INLINE_BYTES 16
#define _MW_STRING_GROWTH_FACTOR(VALUE) VALUE + (VALUE >> 1)

struct mw_string {
	unsigned int cap;
	unsigned int size;
	char* data;
	char inlined_data[_MW_STRING_INLINE_BYTES];
};

typedef struct mw_string mw_string;

static inline unsigned int
_mw_string_rawlen(const char* c) {
	unsigned int len = 0;
	while (*c != '\0') {
		len++;
		c++;
	}
	return len;
}


/*
Reserve capacity for the string. Use this if you know that you are going to deal with a large string.
- Does nothing if new capacity < current capacity. 
- If capacity < _MW_STRING_INLINE_BYTES, it avoids allocating on the heap altogether.
*/
static inline void
mw_string_reserve(mw_string* str, unsigned int new_cap) {

	_mw_set_code(mw_code_ok);

	if (new_cap <= str->cap) {
		return;
	}

	if (new_cap <= _MW_STRING_INLINE_BYTES) {
		str->data = (char*) &str->inlined_data;
	}
	else if (str->data == NULL) {
		str->data = (char*) malloc(sizeof(char) * new_cap);
	} else {
		str->data = (char*) realloc(str->data, new_cap);
	}

	str->cap = new_cap;
}


/*
Resize a string. Fills bytes with '\0' if the size is increased.
- If new size < current size, it truncates the string. 
- If new size > current size, it expands the string and fills with null bytes. 
- If new size is also > capacity, the new capacity will fit the new size.
*/
static inline void
mw_string_resize(mw_string* str, unsigned int new_size) {

	_mw_set_code(mw_code_ok);

	if (new_size <= str->size) {
		memset(str->data + new_size, '\0', str->size - new_size);
		str->size = new_size;
		return;
	}

	if (new_size > str->cap) {
		mw_string_reserve(str, new_size);
		if (!mw_check()) {return;}
	}

	memset(str->data + str->size, '\0', new_size - str->size);
	str->size = new_size;
}

/*
Create a new string from a raw string with it's length.
*/
static inline mw_string 
mw_string_newl(const char* c, unsigned int len) {
	_mw_set_code(mw_code_ok);
	mw_string string;
	mw_string_reserve(&string, len);
	if (!mw_check()) {return string;}
	memcpy(string.data, c, len);
	string.size = len;
	return string;
}

/*
Create a new string from a raw string. It's O(n^2) since the length of the string has to be found too. 
If the length of the raw string is known, use `_mw_string_newl`
*/
static inline mw_string 
mw_string_new(const char* c) {
	return mw_string_newl(c, _mw_string_rawlen(c));
}


/*
Delete a string and its contents.
*/
static inline void
mw_string_delete(mw_string* str) {
	_mw_set_code(mw_code_ok);
	if (str->data != NULL and str->data != (char*)&str->inlined_data) {
		free(str->data);
	}
	str->size = 0;
	str->cap = 0;
}


/*
Initialize a string from a raw string and it's length. Comparable to assignment.
*/
static inline void
mw_string_froml(mw_string* str, const char* c, unsigned int len) {
	_mw_set_code(mw_code_ok);
	mw_string_reserve(str, len);
	if (!mw_check()) {return;}
	memcpy(str->data, c, len);
	str->size = len;
} 


/*
Initialize a string from a raw string. The process is O(n^2) because length has to be found. If length is known or you're looking for 
a more optimized process, use `mw_string_froml` instead.
*/
static inline void
mw_string_from(mw_string* str, const char* c) {
	return mw_string_froml(str, c, _mw_string_rawlen(c));
}

/*
Initialize a string with another string.
*/
static inline void
mw_string_froms(mw_string* str, const mw_string src) {
	return mw_string_froml(str, src.data, src.size);
}

/*
Get the current capacity of the string.
*/
static inline unsigned int 
mw_string_cap(const mw_string* str) {
	return str->cap;
}

/*
Get the current size of the string.
*/
static inline unsigned int 
mw_string_size(const mw_string* str) {
	return str->size;
}

/*
Get the raw data of the string. The raw string is guaranteed to end in `\0` so it is C compatible.
*/
static inline const char*
mw_string_data(mw_string* str) {
	_mw_set_code(mw_code_ok);

	unsigned int pos = str->size;
	mw_string_reserve(str, str->size + 1);
	if (!mw_check()) {return NULL;}
	str->data[pos] = '\0'; 
	// the size is actually not updated here because \0 is just for c safety. 
	return str->data;
}

/*
Get a character at `i` position. Should be used when the index is unknown. If you're iterating over the characters, it's recommended
to just access the raw data using `str.data`. Returns '\0' in case of failure.
*/
static inline char
mw_string_get(const mw_string* str, unsigned int i) {
	_mw_set_code(mw_code_ok);
	if (i >= str->size) {_mw_set_code(mw_code_error); return '\0';}
	return str->data[i];
}

/*
Set a character at `i` position. Should be used when the index is unknown. If you're iterating over the characters, it's recommended
to just access the raw data using `str.data`.
*/
static inline void
mw_string_set(mw_string* str, unsigned int i, char c) {
	_mw_set_code(mw_code_ok);
	if (i >= str->size) {_mw_set_code(mw_code_error); return;};
	str->data[i] = c;
}

/*
Push a character to the end of the string. Will reserve more memory if required.
*/
static inline void
mw_string_push(mw_string* str, char c) {

	_mw_set_code(mw_code_ok);

	if (str->size >= str->cap) {
		mw_string_reserve(str, _MW_STRING_GROWTH_FACTOR(str->cap));
		if (!mw_check()) {return;}
	}

	str->data[str->size++] = c;
}

/*
Pop a character from the end of the string.
*/
static inline char
mw_string_pop(mw_string* str, char c) {

	_mw_set_code(mw_code_ok);

	if (str->size == 0) {
		_mw_set_code(mw_code_error);
		return '\0';
	}

	return str->data[--str->size];;
}

/*
Extend the string with a raw string, reserving more memory if needed. Also takes the length of the raw string.
*/
static inline void
mw_string_extendl(mw_string* str, const char* c, unsigned int len) {

	_mw_set_code(mw_code_ok);

	if (str->size + len >= str->cap) {
		mw_string_reserve(str, _MW_STRING_GROWTH_FACTOR(str->cap) + len);
		if (!mw_check()) {return;}
	}

	memcpy(str->data + str->size, c, len);
	str->size += len;
}

/*
Extend the string with a raw string, reserving more memory if needed. Use `extendl` if the length of the raw string is known.
*/
static inline void 
mw_string_extend(mw_string* str, const char* c) {
	return mw_string_extendl(str, c, _mw_string_rawlen(c));
}

/*
Extend the string with another string, reserving more memory if needed.
*/
static inline void
mw_string_extends(mw_string* str, mw_string* other) {
	return mw_string_extendl(str, other->data, other->size);
}


/*
Create a copy of the string from another string.
*/
static inline mw_string
mw_string_copy(const mw_string* str) {
	_mw_set_code(mw_code_ok);
	mw_string copy = {0};
	mw_string_froml(&copy, str->data, str->size);
	if (!mw_check()) {return copy;}
	return copy;
}

/*
Check whether a string starts with specified raw string. If another string object needs to be used, use `mw_string_data`.
*/
static inline bool
mw_string_startswith(const mw_string* str, const char* c) {

	_mw_set_code(mw_code_ok);

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
mw_string_endswith(const mw_string* str, const char* c) {

	_mw_set_code(mw_code_ok);

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


#endif // MEOWWW_STRING_H