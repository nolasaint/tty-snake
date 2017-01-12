/**
 * engine.h
 *
 * tty-snake game engine.
 *
 * See LICENSE for copyright information.
 */

#ifndef ENGINE_H
#define ENGINE_H

#define ENGINE_MS_PER_TICK 60

#include <global.h>

extern bool is_engine_running;

void engine_start(void);
void engine_stop(void);

#endif // ENGINE_H
