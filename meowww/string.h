#ifndef MEOWWW_STRING_H
#define MEOWWW_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "defs.h"

#define _MW_STRING_INLINE_BYTES 16
#define _MW_STRING_GROWTH_FACTOR(VALUE) VALUE + (VALUE >> 1)

#define mw_string_rawpack(raw_str) raw_str, strlen(raw_str)

struct mw_string {
	unsigned int cap;
	unsigned int size;
	union {
		char  inline_buf[_MW_STRING_INLINE_BYTES];
		char* heap_ptr;
	} data;	
};

typedef struct mw_string mw_string;


/*
Reserve capacity for the string. Use this if you know that you are going to deal with a large string.
- Does nothing if new capacity < current capacity. 
- If capacity < _MW_STRING_INLINE_BYTES, it avoids allocating on the heap altogether.
*/
static inline void
mw_string_reserve(mw_string* str, unsigned int new_cap) {

	if (new_cap <= str->cap) {
		return;
	}

	if (new_cap <= _MW_STRING_INLINE_BYTES) {
		// inline sso
		_mw_log("no malloc!");
	}
	else {

		if (str->data.heap_ptr == NULL)
		str->data.heap_ptr = (char*) malloc(sizeof(char) * new_cap);
		else 
		str->data.heap_ptr = (char*) realloc(str->data.heap_ptr, new_cap);

		_mw_assert(str->data.heap_ptr == NULL, "Memory could not be allocated for string %p.", str);
	}

	str->cap = new_cap;
}


/*
Resolve the data pointer of a string.
*/
static inline char*
_mw_string_get_data(const mw_string* str) {
	if (str->cap <= _MW_STRING_INLINE_BYTES) {
		return (char*) &str->data.inline_buf;
	}
	_mw_assert(str->data.heap_ptr != NULL, "Cannot get data pointer of string (%p), is null.", str);
	return str->data.heap_ptr;
}


/*
Resize a string. Fills bytes with '\0' if the size is increased.
- If new size < current size, it truncates the string. 
- If new size > current size, it expands the string and fills with null bytes. 
- If new size is also > capacity, the new capacity will fit the new size.
*/
static inline void
mw_string_resize(mw_string* str, unsigned int new_size) {

	char* data = _mw_string_get_data(str);

	if (new_size <= str->size) {
		memset(data + new_size, '\0', str->size - new_size);
		str->size = new_size;
		return;
	}

	if (new_size > str->cap) {
		mw_string_reserve(str, new_size);
	}

	memset(data + str->size, '\0', new_size - str->size);
	str->size = new_size;
}

/*
Create a new string from a raw string with it's length.
*/
static inline mw_string 
mw_string_new(const char* c, unsigned int len) {
	mw_string string;
	mw_string_reserve(&string, len);
	memcpy(_mw_string_get_data(&string), c, len);
	string.size = len;
	return string;
}

/*
Create a new string from a raw string. It's O(n^2) since the length of the string has to be found too. 
If the length of the raw string is known, use `_mw_string_new`
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
	char* data = _mw_string_get_data(str);
	if (data != NULL && data != (char*)&str->data.inline_buf) {
		free(str->data.heap_ptr);
	}
	str->size = 0;
	str->cap = 0;
}


/*
Assign a string from a raw string and it's length.
*/
static inline void
mw_string_from(mw_string* str, const char* c, unsigned int len) {
	mw_string_reserve(str, len);
	memcpy(_mw_string_get_data(str), c, len);
	str->size = len;
} 


/*
Assign a string from a raw string. The process is O(n^2) because length has to be found. If length is known or you're looking for 
a more optimized process, use `mw_string_froml` instead.
*/
static inline void
mw_string_fromc(mw_string* str, const char* c) {
	return mw_string_from(str, c, strlen(c));
}

/*
Assign a string with another string.
*/
static inline void
mw_string_froms(mw_string* str, const mw_string* src) {
	return mw_string_from(str, _mw_string_get_data(src), src->size);
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
	unsigned int pos = str->size;
	mw_string_reserve(str, str->size + 1); // this can cause some slowness if str.cap happens to be equal to str.size
	char* c = _mw_string_get_data(str);
	c[pos] = '\0'; 
	// the size is actually not updated here because \0 is just for c safety. 
	return c;
}

/*
Get a character at `i` position. Should be used when the index is unknown. If you're iterating over the characters, it's recommended
to just access the raw data using `mw_string_data`. Returns '\0' in case of failure.
*/
static inline char
mw_string_get(const mw_string* str, unsigned int i) {
	_mw_assert(i < str->size, "Out of bounds access. String is of length %u, cannot access index %u.", str->size, i);
	return _mw_string_get_data(str)[i];
}

/*
Set a character at `i` position. Should be used when the index is unknown. If you're iterating over the characters, it's recommended
to just access the raw data using `mw_string_data`.
*/
static inline void
mw_string_set(mw_string* str, unsigned int i, char c) {
	_mw_assert(i < str->size, "Out of bounds access. String is of length %u, cannot access index %u.", str->size, i);
	_mw_string_get_data(str)[i] = c;
}

/*
Push a character to the end of the string. Will reserve more memory if required.
*/
static inline void
mw_string_push(mw_string* str, char c) {

	if (str->size >= str->cap) {
		mw_string_reserve(str, _MW_STRING_GROWTH_FACTOR(str->cap));
	}

	_mw_string_get_data(str)[str->size++] = c;
}

/*
Pop a character from the end of the string.
*/
static inline char
mw_string_pop(mw_string* str, char c) {
	_mw_assert(str->size != 0, "Pop out of empty string (%p) not possible.", str);
	return _mw_string_get_data(str)[--str->size];;
}

/*
Extend the string with a raw string, reserving more memory if needed. Also takes the length of the raw string.
*/
static inline void
mw_string_extend(mw_string* str, const char* c, unsigned int len) {

	if (str->size + len >= str->cap) {
		mw_string_reserve(str, _MW_STRING_GROWTH_FACTOR(str->cap) + len);
	}

	memcpy(_mw_string_get_data(str) + str->size, c, len);
	str->size += len;
}

/*
Extend the string with another string, reserving more memory if needed.
*/
static inline void
mw_string_extends(mw_string* str, mw_string* other) {
	return mw_string_extend(str, _mw_string_get_data(other), other->size);
}


/*
Check whether a string starts with specified raw string. If another string object needs to be used, use `mw_string_data`.
*/
static inline bool
mw_string_startswith(const mw_string* str, const char* c, unsigned int len) {

	unsigned int index = 0;
	char* curr = (char*)c;
	char* data = _mw_string_get_data(str);

	while (*curr != '\0') {

		if (index >= str->size) {
			return false;
		}

		if (*curr != data[index]) {
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
mw_string_endswith(const mw_string* str, const char* c, unsigned int len) {

	unsigned int index = str->size - len;

	char* curr = (char*) c;
	char* data = _mw_string_get_data(str);

	while (*curr != '\0') {

		if (index >= str->size) {
			return false;
		}

		if (*curr != data[index]) {
			return false;
		}

		index++;
		curr++;
	}
	return true;
}

/*
Finds the number of occurrences of a sub string. Returns 0 if none are found. If the sub string is larger, it also returns 0
*/
static inline unsigned int
mw_string_count(const mw_string* str, const char* c, unsigned int len) {

	if (len > str->size) return 0;

	unsigned int count = 0;
	char* data = _mw_string_get_data(str);

	for (unsigned int i = 0; i < str->size; i++) {
		for (unsigned int j = 0; j < len; j++) {
			if (data[i+j] != c[j]) break;
		}
		count++;
	}

	return count;
}

/*
Finds the number of occurrences of a sub string. Returns 0 if none are found.
*/
static inline unsigned int
mw_string_counts(const mw_string* str, const mw_string* other) {
	return mw_string_count(str, _mw_string_get_data(other), other->size);
}



#endif // MEOWWW_STRING_H