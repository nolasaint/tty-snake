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
 * function:  ns2timespec
 * ----------------------
 * TODO - documentation
 */
inline void ns2timespec(nanosecond_t ns, struct timespec *ts)
{
  // avoid segmentation faults
  if (ts)
  {
    ts->tv_sec  = ns / SECONDS;
    ts->tv_nsec = ns % SECONDS;
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
