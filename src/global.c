/**
 * global.c
 *
 * globally-used functions, definitions, and imports.
 *
 * See LICENSE for copyright information.
 */

#include <stdlib.h>

#include <global.h>

/**
 * function:  ms2timespec
 * ----------------------
 * TODO - documentation
 */
inline void ms2timespec(long ms, struct timespec *ts)
{
  // avoid segmentation faults
  if (ts)
  {
    ts->tv_sec  = MS2S(ms);
    ts->tv_nsec = MS2NS(ms) - MS2NS(S2MS((ts->tv_sec)));
  }
}

/**
 * function:  quit
 * ---------------
 * exit the program.
 */
void quit(void)
{
  // TODO should try to signal engine to stop first?
  exit(EXIT_FAILURE);
}
