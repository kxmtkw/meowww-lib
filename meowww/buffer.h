#ifndef MEOWWW_BUFFER_H
#define MEOWWW_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "internal/defs.h"
#include "internal/config.h"
#include "internal/types.h"

/*
Reserve capacity for the buffer.
- Does nothing if new capacity <= current capacity. 
- If capacity <= MW_BUFFER_INLINE_DATA_SIZE, it avoids heap allocation.
*/
static inline void
mw_buffer_reserve(mw_buffer* buf, unsigned int new_cap) {

	if (new_cap <= buf->cap) {
		return;
	}

	if (new_cap <= MW_BUFFER_INLINE_DATA_SIZE) {
		_mw_log("no malloc!");
	}
	else {
		if (buf->data.heap_ptr == NULL)
		buf->data.heap_ptr = (mw_byte*) malloc(sizeof(mw_byte) * new_cap);
		else
		buf->data.heap_ptr = (mw_byte*) realloc(buf->data.heap_ptr, sizeof(mw_byte) * new_cap);

		_mw_assert(buf->data.heap_ptr != NULL, "Memory could not be allocated for buffer %p.", buf);
	}

	buf->cap = new_cap;
}

/*
Resolve the data pointer of a buffer.
*/
static inline mw_byte*
_mw_buffer_get_data(const mw_buffer* buf) {
	if (buf->cap <= MW_BUFFER_INLINE_DATA_SIZE) {
		return (mw_byte*) &buf->data.inline_buf;
	}
	_mw_assert(buf->data.heap_ptr != NULL, "Cannot get data pointer of buffer (%p), is null.", buf);
	return buf->data.heap_ptr;
}

/*
Resize a buffer. Fills bytes with '\0' if the size is increased.
*/
static inline void
mw_buffer_resize(mw_buffer* buf, unsigned int new_size) {

	mw_byte* data = _mw_buffer_get_data(buf);

	if (new_size <= buf->size) {
		memset(data + new_size, 0, buf->size - new_size);
		buf->size = new_size;
		return;
	}

	if (new_size > buf->cap) {
		mw_buffer_reserve(buf, new_size);
		data = _mw_buffer_get_data(buf);
	}

	memset(data + buf->size, '\0', new_size - buf->size);
	buf->size = new_size;
}


/*
Create a new buffer from a byte array.
*/
static inline mw_buffer 
mw_buffer_new(const mw_byte* b, unsigned int len) {
	mw_buffer buf;
	mw_buffer_reserve(&buf, len);
	memcpy(_mw_buffer_get_data(&buf), b, len);
	buf.size = len;
	return buf;
}

/*
Create a new buffer from another buffer.
*/
static inline mw_buffer 
mw_buffer_news(const mw_buffer* src) {
	return mw_buffer_new(_mw_buffer_get_data(src), src->size);
}

/*
Create a new zero filled buffer with the given size.
*/
static inline mw_buffer 
mw_buffer_newl(unsigned int size) {
	mw_buffer buf;
	memset(&buf, 0, sizeof(mw_buffer));
	mw_buffer_resize(&buf, size);
	return buf;
}


/*
Delete a buffer and free its heap memory if allocated.
*/
static inline void
mw_buffer_delete(mw_buffer* buf) {
	if (buf->cap > MW_BUFFER_INLINE_DATA_SIZE && buf->data.heap_ptr != NULL) {
		free(buf->data.heap_ptr);
		buf->data.heap_ptr = NULL;
	}
	buf->size = 0;
	buf->cap = 0;
}

/*
Assign a buffer from raw bytes and a length.
*/
static inline void
mw_buffer_from(mw_buffer* buf, const mw_byte* bytes, unsigned int len) {
	mw_buffer_reserve(buf, len);
	memcpy(_mw_buffer_get_data(buf), bytes, len);
	buf->size = len;
} 

/*
Assign a buffer from another buffer. Is basically a copy but uses the destination buffer's already allocated memory.
*/
static inline void
mw_buffer_fromb(mw_buffer* buf, const mw_buffer* src) {
	mw_buffer_from(buf, _mw_buffer_get_data(src), src->size);
}

/*
Get the current capacity of the buffer.
*/
static inline unsigned int 
mw_buffer_cap(const mw_buffer* buf) {
	return buf->cap;
}

/*
Get the current size of the buffer.
*/
static inline unsigned int 
mw_buffer_size(const mw_buffer* buf) {
	return buf->size;
}

/*
Get a byte at position `i`.
*/
static inline mw_byte
mw_buffer_get(const mw_buffer* buf, unsigned int i) {
	_mw_assert(i < buf->size, "Out of bounds access. Buffer is of length %u, cannot access index %u.", buf->size, i);
	return _mw_buffer_get_data(buf)[i];
}

/*
Set a byte at position `i`.
*/
static inline void
mw_buffer_set(mw_buffer* buf, unsigned int i, mw_byte b) {
	_mw_assert(i < buf->size, "Out of bounds access. Buffer is of length %u, cannot access index %u.", buf->size, i);
	_mw_buffer_get_data(buf)[i] = b;
}

/*
Push a byte to the end of the buffer.
*/
static inline void
mw_buffer_push(mw_buffer* buf, mw_byte b) {
	if (buf->size >= buf->cap) {
		mw_buffer_reserve(buf, MW_BUFFER_GROWTH_FACTOR(buf->cap));
	}
	_mw_buffer_get_data(buf)[buf->size++] = b;
}

/*
Pop a byte from the end of the buffer.
*/
static inline mw_byte
mw_buffer_pop(mw_buffer* buf) {
	_mw_assert(buf->size > 0, "Pop out of empty buffer (%p) not possible.", buf);
	return _mw_buffer_get_data(buf)[--buf->size];
}

/*
Extend the buffer with raw bytes and a length.
*/
static inline void
mw_buffer_extend(mw_buffer* buf, const mw_byte* bytes, unsigned int len) {
	if (buf->size + len > buf->cap) {
		mw_buffer_reserve(buf, buf->size + len);
	}
	memcpy(_mw_buffer_get_data(buf) + buf->size, bytes, len);
	buf->size += len;
}

/*
Extend the buffer with another buffer.
*/
static inline void
mw_buffer_extendb(mw_buffer* buf, const mw_buffer* other) {
	mw_buffer_extend(buf, _mw_buffer_get_data(other), other->size);
}

/*
Finds the number of occurrences of a byte sequence. Returns 0 if none are found.
*/
static inline unsigned int
mw_buffer_count(const mw_buffer* buf, const mw_byte* bytes, unsigned int len) {
	if (len == 0 || len > buf->size) return 0;

	unsigned int count = 0;
	mw_byte* data = _mw_buffer_get_data(buf);

	for (unsigned int i = 0; i <= buf->size - len; i++) {
		if (memcmp(data + i, bytes, len) == 0) {
			count++;
			i += len - 1; // Non-overlapping count increment
		}
	}

	return count;
}

/*
Finds the number of occurrences of another buffer's contents.
*/
static inline unsigned int
mw_buffer_countb(const mw_buffer* buf, const mw_buffer* other) {
	return mw_buffer_count(buf, _mw_buffer_get_data(other), other->size);
}

/*
Finds the first index after `start` that matches the given byte sequence.
*/
static inline bool
mw_buffer_find(const mw_buffer* buf, const mw_byte* bytes, unsigned int len, unsigned int start, unsigned int* index) {
	if (len == 0 || start >= buf->size || len > buf->size - start) return false;

	mw_byte* data = _mw_buffer_get_data(buf);

	for (unsigned int i = start; i <= buf->size - len; i++) {
		if (memcmp(data + i, bytes, len) == 0) {
			if (index != NULL) *index = i;
			return true;
		}
	}

	return false;
}

/*
Finds the first index after `start` that matches another buffer's contents.
*/
static inline bool
mw_buffer_findb(const mw_buffer* buf, const mw_buffer* subbuf, unsigned int start, unsigned int* index) {
	return mw_buffer_find(buf, _mw_buffer_get_data(subbuf), subbuf->size, start, index);
}

/*
Check whether a buffer starts with a specified byte sequence.
*/
static inline bool
mw_buffer_startswith(const mw_buffer* buf, const mw_byte* bytes, unsigned int len) {
	if (len > buf->size) return false;
	return memcmp(_mw_buffer_get_data(buf), bytes, len) == 0;
}

/*
Check whether a buffer ends with a specified byte sequence.
*/
static inline bool
mw_buffer_endswith(const mw_buffer* buf, const mw_byte* bytes, unsigned int len) {
	if (len > buf->size) return false;
	return memcmp(_mw_buffer_get_data(buf) + (buf->size - len), bytes, len) == 0;
}

#endif // MEOWWW_BUFFER_H