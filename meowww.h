#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEOWWW_IMPLEMENT

#ifndef MEOWWWW_LIB_H
#define MEOWWWW_LIB_H

typedef enum {
	mw_code_ok,
	mw_code_error
} mw_code;

/*

string defintions

*/

struct __mw_string;
typedef struct __mw_string mw_string;

/* Create a new empty string object */
static inline mw_string 
mw_string_new();

/* Delete the string object */
static inline mw_code
mw_string_delete(mw_string* s);

/* Initializes the string with `c`. Clears previous data. To make from another mw_string object, use `mw_string_data`*/
static inline mw_code
mw_string_from(mw_string* s, const char* c);

/* Get the capacity of the string */
static inline unsigned int 
mw_string_cap(mw_string* s);

/* Get the size of the string */
static inline unsigned int 
mw_string_size(mw_string* s);

/* Get the raw data buffer of the string. It is assured that the string is null terminated. */
static inline const char*
mw_string_data(mw_string* s);

/* Reserve `c` capacity for the string. If the `c` < current capacity, nothing happens.*/
static inline mw_code
mw_string_reserve(mw_string* s, unsigned int c);

/* Resize the string to size `c`. If `c` < current size, it truncates the string. If `c` > current size, 
it fills with null characters. If `c` > current capacity, it increases capacity to fit the new size.*/
static inline mw_code
mw_string_resize(mw_string* s, unsigned int c);

/* Writes the character from index `i` to `c`.*/
static inline mw_code
mw_string_get(mw_string* s, unsigned int i, char* c);

/* Sets the character at index `i` to `c`.*/
static inline mw_code
mw_string_set(mw_string* s, unsigned int i, char c);

/* Push a character to the end.*/
static inline mw_code
mw_string_push(mw_string* s, char c);


/*

array defintions

*/

struct __mw_raw_array;
typedef struct __mw_raw_array mw_raw_array;

/* Create a new array with a given size and element size.*/
static inline mw_raw_array
mw_raw_array_new(unsigned int size, unsigned int elem_size);

/* Sets the contents of the array with `data`. `data` must have enough content to fill the array.*/
static inline mw_code
mw_raw_array_from(mw_raw_array* array, void* data);

/* Delete the array object. Freeing pointers stored inside the array is the user's responsibility.*/
static inline mw_code
mw_raw_array_delete(mw_raw_array* array);

/* Get the size of the array. */
static inline unsigned int
mw_raw_array_size(mw_raw_array* array);

/* Writes the element at index `i` to location `ret`. `ret` must be capable of holding element size.*/
static inline mw_code
mw_raw_array_get(mw_raw_array* array, unsigned int i, void* ret);

/* Sets the value at index `i` with whatever is present in `val`. `val` must be of element size.*/
static inline mw_code
mw_raw_array_set(mw_raw_array* array, unsigned int i, void* val);


/*

custom array defintion

*/

#define mw_define_array(NAME, TYPE) \
typedef struct { TYPE* data; unsigned int size; unsigned int elem_size; } mw_ ## NAME ## _array; \
\
/* Create a new array with the given size. */ \
static inline mw_ ## NAME ## _array \
mw_ ## NAME ## _array_new(unsigned int size) { \
	mw_raw_array raw = mw_raw_array_new(size, sizeof(TYPE)); \
	mw_ ## NAME ## _array arr = { \
		.data = (TYPE*)raw.data, \
		.size = raw.size, \
		.elem_size = raw.elem_size \
	}; \
	return arr; \
} \
/* Delete the array. */ \
static inline mw_code \
mw_ ## NAME ## _array_from(mw_ ## NAME ## _array* arr, void* data) { return mw_raw_array_from((mw_raw_array*)arr, data); } \
\
\
/* Delete the array. */ \
static inline mw_code \
mw_ ## NAME ## _array_delete(mw_ ## NAME ## _array* arr) { return mw_raw_array_delete((mw_raw_array*)arr); } \
\
\
/* Get the size of the array. */ \
static inline unsigned int \
mw_ ## NAME ## _array_size(mw_ ## NAME ## _array* arr) { return arr->size; } \
\
/* Safe read with bounds checking */ \
static inline mw_code \
mw_ ## NAME ## _array_get(mw_ ## NAME ## _array* arr, unsigned int i, TYPE* ret) { \
	return mw_raw_array_get((mw_raw_array*)arr, i, ret); \
} \
\
/* Unsafe fast read (unchecked) */ \
static inline TYPE \
mw_ ## NAME ## _array_getu(mw_ ## NAME ## _array* arr, unsigned int i) { \
	return arr->data[i]; \
} \
\
/* Safe write with bounds checking */ \
static inline mw_code \
mw_ ## NAME ## _array_set(mw_ ## NAME ## _array* arr, unsigned int i, TYPE val) { \
	return mw_raw_array_set((mw_raw_array*)arr, i, &val); \
} \
\
/* Unsafe fast write (unchecked) */ \
static inline void \
mw_ ## NAME ## _array_setu(mw_ ## NAME ## _array* arr, unsigned int i, TYPE val) { \
	arr->data[i] = val; \
}


#ifdef MEOWWW_IMPLEMENT


/*

string implementation

*/

struct __mw_string {
	char* data;
	unsigned int cap;
	unsigned int size;
};


static inline mw_string 
mw_string_new() {
	return (mw_string) {
		NULL,
		0,
		0,
	};
}

static inline mw_code
mw_string_delete(mw_string* s) {
	if (s->data) {
		free(s->data);
	}
	s->size = 0;
	s->cap = 0;
	return mw_code_ok;
}


static inline mw_code
mw_string_from(mw_string* s, const char* c) {

	unsigned int len = 0;
	char* curr_c = (char*) c;

	while (*curr_c != '\0') {
		len++;
		curr_c++;
	}

	mw_string_reserve(s, len);

	memcpy(s->data, c, len);
	s->size = len;

	return mw_code_ok;
}


static inline unsigned int 
mw_string_cap(mw_string* s) {
	return s->cap;
}


static inline unsigned int 
mw_string_size(mw_string* s) {
	return s->size;
}


static inline const char*
mw_string_data(mw_string* s) {
	// the size is not updated. 
	unsigned int pos = s->size;
	mw_string_reserve(s, s->size + 1);
	s->data[pos] = '\0'; 
	return s->data;
}


static inline mw_code
mw_string_reserve(mw_string* s, unsigned int c) {

	if (c <= s->cap) {
		return mw_code_ok;
	}

	if (s->data == NULL) {
		s->data = (char*) malloc(sizeof(char) * c);
	} else {
		s->data = (char*) realloc(s->data, c);
	}

	s->cap = c;

	return mw_code_ok;
}


static inline mw_code
mw_string_resize(mw_string* s, unsigned int c) {

	if (c <= s->size) {
		s->size = c;
		return mw_code_ok;
	}

	if (c > s->cap) {
		mw_string_reserve(s, c);
	}

	memset(s->data + s->size, '\0', c - s->size);
	s->size = c;

	return mw_code_ok;
}


static inline mw_code
mw_string_get(mw_string* s, unsigned int i, char* c) {
	if (i >= s->size) return mw_code_error;
	*c = s->data[i];
	return mw_code_ok;
}


static inline mw_code
mw_string_set(mw_string* s, unsigned int i, char c) {
	if (i >= s->size) return mw_code_error;
	s->data[i] = c;
	return mw_code_ok;
}

static inline mw_code
mw_string_push(mw_string* s, char c) {
	if (s->size >= s->cap) {
		mw_string_reserve(s, s->cap + (s->cap >> 1));
	}

	s->data[s->size++] = c;

	return mw_code_ok;
}


/*

raw array implementation

*/

struct __mw_raw_array {
	uint8_t* data;
	unsigned int size;
	unsigned int elem_size;
};


static inline mw_raw_array
mw_raw_array_new(unsigned int size, unsigned int elem_size) {
	mw_raw_array arr;
	arr.data = (uint8_t*) malloc(size * elem_size);
	arr.size = size;
	arr.elem_size = elem_size;
	return arr;
}

static inline mw_code
mw_raw_array_from(mw_raw_array* array, void* data) {
	memcpy(array->data, data, array->size * array->elem_size);
	return mw_code_ok;
}

static inline mw_code
mw_raw_array_delete(mw_raw_array* array) {
	if (array->data) {
		free(array->data);
	}

	array->size = 0;
	array->elem_size = 0;

	return mw_code_ok;
}

static inline unsigned int
mw_raw_array_size(mw_raw_array* array) {
	return array->size;
}

static inline mw_code
mw_raw_array_get(mw_raw_array* array, unsigned int i, void* ret) {

	if (i >= array->size) {
		return mw_code_error;
	}

	uint8_t* value = array->data + (i * array->elem_size);
	memcpy(ret, value, array->elem_size);

	return mw_code_ok;
}

static inline mw_code
mw_raw_array_set(mw_raw_array* array, unsigned int i, void* val) {

	if (i >= array->size) {
		return mw_code_error;
	}

	uint8_t* value = array->data + (i * array->elem_size);
	memcpy(value, val, array->elem_size);

	return mw_code_ok;
}


#endif // MEOWWW_IMPLEMENT


#endif // MEOWWW_LIB_H