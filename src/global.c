/**
 * global.c
 *
 * globally-used functions, definitions, and imports.
 *
 * See LICENSE for copyright information.
 */

#include <stdlib.h>

#include <global.h>

/*
 * function:  get_time_ms
 * ----------------------
 * TODO - documentation
 */
millisecond_t get_time_ms(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_ID, &ts);

  return (millisecond_t) TIMESPEC2MS(ts);
}

/*
 * function:  get_time_ns
 * ----------------------
 * TODO - documentation
 */
nanosecond_t get_time_ns(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_ID, &ts);

  return (nanosecond_t) TIMESPEC2NS(ts);
}

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
