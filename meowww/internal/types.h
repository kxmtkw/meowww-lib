#ifndef MEOWWW_TYPES_H
#define MEOWWW_TYPES_H

#include <stdint.h>

#include "config.h"

typedef uint8_t mw_byte;

struct mw_buffer {
	unsigned int size;
	unsigned int cap;
	union {
		mw_byte* heap_ptr;
		mw_byte  inline_buf[MW_BUFFER_INLINE_DATA_SIZE]; 
	} data;
};
typedef struct mw_buffer mw_buffer;


struct mw_string {
	unsigned int size;
	unsigned int cap;
	union {
		uint8_t* heap_ptr;
		uint8_t  inline_buf[MW_BUFFER_INLINE_DATA_SIZE]; 
	} data;
};
typedef struct mw_string mw_string;

// mw_string and mw_buffer MUST have the same structure


#endif // MEOWWW_TYPES_H