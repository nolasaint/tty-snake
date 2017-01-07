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

// if defined, debugging output will be compiled
#define DEBUG

void quit(void);

#endif // GLOBAL_H
