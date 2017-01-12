/**
 * global.h
 *
 * globally-used functions, definitions, and imports.
 *
 * See LICENSE for copyright information.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

// compilation options
//#define USE_KB_LISTEN_THREAD

// generic definitions and typedefs
#ifndef bool
typedef _Bool bool;
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

// time conversions
#define NS2MS(ns)       ((ns) / 1000000)
#define MS2S(ms)        ((ms) / 1000)

#define MS2NS(ms)       ((ms) * 1000000)
#define S2MS(s)         ((s) * 1000)

#define TIMESPEC2MS(ts) (S2MS((ts).tv_sec) + NS2MS((ts).tv_nsec))

// if defined, debugging output will be compiled
#define DEBUG

#include <time.h>

inline void ms2timespec(long ms, struct timespec *ts);
void        quit(void);

#endif // GLOBAL_H
