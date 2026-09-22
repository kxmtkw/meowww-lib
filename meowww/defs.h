#ifndef MEOWWW_DEFS_H
#define MEOWWW_DEFS_H

#include <stdio.h>
#include <threads.h>

typedef enum {
	mw_code_ok,
	mw_code_error
} mw_code;


thread_local static mw_code _mw_global_error_code;


static inline bool
mw_check(void) {
	return _mw_global_error_code == mw_code_ok;
}

static inline mw_code
mw_get_code(void) {
	return _mw_global_error_code;
}

static inline void
_mw_set_code(mw_code code) {
	_mw_global_error_code = code;
	#ifdef MEOWWW_RAISE_ERRORS
	if (code != mw_code_ok) {
		printf("[meowww] failure\n");
		exit(1);
	}
	#endif 
}


#endif // MEOWWW_DEFS_H