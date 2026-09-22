#ifndef MEOWWW_ARRAY_H
#define MEOWWW_ARRAY_H

#include "defs.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


struct _mw_array {
	uint8_t* data;
	const unsigned int size;
	const unsigned int elem_size;
};

typedef struct _mw_array _mw_array;


static inline _mw_array
_mw_array_new(unsigned int size, unsigned int elem_size) {
	uint8_t* data = (uint8_t*) malloc(size * elem_size);
	_mw_array arr = {
		data,
		size,
		elem_size
	};
	return arr;
}

static inline void
_mw_array_from(_mw_array* array, void* data) {
	_mw_set_code(mw_code_ok);
	memcpy(array->data, data, array->size * array->elem_size);
}

static inline void
_mw_array_delete(_mw_array* array) {
	_mw_set_code(mw_code_ok);
	if (array->data) {
		free(array->data);
	}
	array->data = NULL;
}

static inline unsigned int
_mw_array_size(_mw_array* array) {
	return array->size;
}

static inline void
_mw_array_get(_mw_array* array, unsigned int i, void* ret) {

	_mw_set_code(mw_code_ok);

	if (array->data == NULL) {
		_mw_set_code(mw_code_error);
		return;
	}
	
	if (i >= array->size) {
		_mw_set_code(mw_code_error);;
		return;
	}

	uint8_t* value = array->data + (i * array->elem_size);
	memcpy(ret, value, array->elem_size);
}

static inline void
_mw_array_set(_mw_array* array, unsigned int i, void* val) {

	_mw_set_code(mw_code_ok);

	if (array->data == NULL) {
		_mw_set_code(mw_code_error);
		return;	
	}

	if (i >= array->size) {
		_mw_set_code(mw_code_error);
		return;
	}

	uint8_t* value = array->data + (i * array->elem_size);
	memcpy(value, val, array->elem_size);
}


#define _mw_define_array_with_prefix(PREFIX, NAME, TYPE) \
typedef struct { TYPE* data; unsigned int size; unsigned int elem_size; } PREFIX ## mw_ ## NAME ## _array; \
\
static inline PREFIX ## mw_ ## NAME ## _array \
PREFIX ## mw_ ## NAME ## _array_new(unsigned int size) { \
    _mw_array raw = _mw_array_new(size, sizeof(TYPE)); \
    PREFIX ## mw_ ## NAME ## _array arr = { \
        .data = (TYPE*)raw.data, \
        .size = raw.size, \
        .elem_size = raw.elem_size \
    }; \
    return arr; \
} \
static inline PREFIX ## mw_ ## NAME ## _array \
PREFIX ## mw_ ## NAME ## _array_newl(unsigned int size, void* data) { \
    PREFIX ## mw_ ## NAME ## _array arr = PREFIX ## mw_ ## NAME ## _array_new(size); \
	if (!mw_check()) {return arr;} \
	_mw_array_from((_mw_array*)&arr, data); \
    return arr; \
} \
static inline void \
PREFIX ## mw_ ## NAME ## _array_from(PREFIX ## mw_ ## NAME ## _array* arr, TYPE data[]) { _mw_array_from((_mw_array*)arr, data); } \
\
static inline void \
PREFIX ## mw_ ## NAME ## _array_delete(PREFIX ## mw_ ## NAME ## _array* arr) { _mw_array_delete((_mw_array*)arr); } \
\
static inline unsigned int \
PREFIX ## mw_ ## NAME ## _array_size(PREFIX ## mw_ ## NAME ## _array* arr) { return arr->size; } \
\
static inline TYPE \
PREFIX ## mw_ ## NAME ## _array_get(PREFIX ## mw_ ## NAME ## _array* arr, unsigned int i) { \
	TYPE ret; \
    _mw_array_get((_mw_array*)arr, i, &ret); \
	return ret; \
} \
\
static inline void \
PREFIX ## mw_ ## NAME ## _array_set(PREFIX ## mw_ ## NAME ## _array* arr, unsigned int i, TYPE val) { \
    _mw_array_set((_mw_array*)arr, i, &val); \
}


#define mw_define_array(NAME, TYPE) _mw_define_array_with_prefix(,NAME,TYPE)
#define _mw_define_internal_array(NAME, TYPE) _mw_define_array_with_prefix(_,NAME,TYPE)

mw_define_array(int, int)
mw_define_array(float, float)

#endif // MEOWWW_ARRAY_H
