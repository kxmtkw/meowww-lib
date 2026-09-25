#ifndef MEOWWW_DEFS_H
#define MEOWWW_DEFS_H

#include <stdio.h>

/*
MEOWWW_DEBUG -> will print debug logs for dev
MEOWWW_NO_ASSERTIONS -> will disable assertions entirely
*/

#ifdef MEOWWW_DEBUG

#define _mw_log(fmt, ...) \
    fprintf(stderr, "[mw] " fmt "\n", ##__VA_ARGS__);

#else

#define _mw_log(fmt, ...) \
    while (0) {};

#endif // MEOWWW_DEBUG


#ifndef MEOWWW_NO_ASSERTIONS 

#define _mw_assert(condition, fmt, ...) \
	if (!(condition)) {fprintf(stderr, "[mw-error] " fmt "\n", ##__VA_ARGS__);}

#else

#define _mw_assert(condition, fmt, ...) \
	while (0) {};


#endif // MEOWWW_NO_ASSERTIONS


#endif // MEOWWW_DEFS_H