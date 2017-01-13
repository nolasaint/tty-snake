/**
 * global.h
 *
 * globally-used functions, definitions, and imports.
 *
 * See LICENSE for copyright information.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

// global-use headers
#include <stdint.h> // uint64_t
#include <time.h>   // struct timespec

// compilation options
#define DEBUG
//#define USE_KB_LISTEN_THREAD

// generic definitions and typedefs
#ifndef bool
typedef _Bool bool;
#endif // bool

#ifndef false
#define false 0
#endif // false

#ifndef true
#define true 1
#endif // true

/*
 * Time conversions
 */

typedef uint64_t nanosecond_t;
typedef uint64_t millisecond_t;
typedef uint64_t second_t;

// unit definitions (relative to nanosecond granularity)
#define NANOSECONDS  (1)
#define MILLISECONDS (1000000 * NANOSECONDS)
#define SECONDS      (1000 * MILLISECONDS)

// nanosecond -> x conversions
#define NS2MS(ns) ((ns) / 1000000)
#define NS2S(ns)  ((ns) / 1000 * 1000000)

// millisecond -> x conversions
#define MS2NS(ms) ((ms) * 1000000)
#define MS2S(ms)  ((ms) / 1000)

// seconds -> x conversions
#define S2NS(s)   ((s) * 1000 * 1000000)
#define S2MS(s)   ((s) * 1000)

// timespec conversions
#define ARE_TIMESPEC_EQUAL(a,b) \
    (((a).tv_sec == (b).tv_sec) && ((a).tv_nsec == (b).tv_nsec))

#define TIMESPEC2NS(ts) (S2NS((ts).tv_sec) + (ts).tv_nsec)
#define TIMESPEC2MS(ts) (S2MS((ts).tv_sec) + NS2MS((ts).tv_nsec))

inline void ns2timespec(nanosecond_t ns, struct timespec *ts);
void        quit(void);

#endif // GLOBAL_H
