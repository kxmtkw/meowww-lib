#pragma once

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

static inline mw_code
_mw_array_from(_mw_array* array, void* data) {
	memcpy(array->data, data, array->size * array->elem_size);
	return mw_code_ok;
}

static inline mw_code
_mw_array_delete(_mw_array* array) {
	if (array->data) {
		free(array->data);
	}
	array->data = NULL;
	return mw_code_ok;
}

static inline unsigned int
_mw_array_size(_mw_array* array) {
	return array->size;
}

static inline mw_code
_mw_array_get(_mw_array* array, unsigned int i, void* ret) {

	if (array->data == NULL) {
		return mw_code_error;	
	}
	
	if (i >= array->size) {
		return mw_code_error;
	}

	uint8_t* value = array->data + (i * array->elem_size);
	memcpy(ret, value, array->elem_size);

	return mw_code_ok;
}

static inline mw_code
_mw_array_set(_mw_array* array, unsigned int i, void* val) {

	if (array->data == NULL) {
		return mw_code_error;	
	}

	if (i >= array->size) {
		return mw_code_error;
	}

	uint8_t* value = array->data + (i * array->elem_size);
	memcpy(value, val, array->elem_size);

	return mw_code_ok;
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
static inline mw_code \
PREFIX ## mw_ ## NAME ## _array_from(PREFIX ## mw_ ## NAME ## _array* arr, void* data) { return _mw_array_from((_mw_array*)arr, data); } \
\
static inline mw_code \
PREFIX ## mw_ ## NAME ## _array_delete(PREFIX ## mw_ ## NAME ## _array* arr) { return _mw_array_delete((_mw_array*)arr); } \
\
static inline unsigned int \
PREFIX ## mw_ ## NAME ## _array_size(PREFIX ## mw_ ## NAME ## _array* arr) { return arr->size; } \
\
static inline mw_code \
PREFIX ## mw_ ## NAME ## _array_get(PREFIX ## mw_ ## NAME ## _array* arr, unsigned int i, TYPE* ret) { \
    return _mw_array_get((_mw_array*)arr, i, ret); \
} \
\
static inline mw_code \
PREFIX ## mw_ ## NAME ## _array_set(PREFIX ## mw_ ## NAME ## _array* arr, unsigned int i, TYPE val) { \
    return _mw_array_set((_mw_array*)arr, i, &val); \
}


#define mw_define_array(NAME, TYPE) _mw_define_array_with_prefix(,NAME,TYPE)
#define _mw_define_internal_array(NAME, TYPE) _mw_define_array_with_prefix(_,NAME,TYPE)