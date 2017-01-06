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
 * function:  quit
 * ---------------
 * exit the program.
 */
void quit(void)
{
  // TODO should try to signal engine to stop first?
  exit(EXIT_FAILURE);
}
