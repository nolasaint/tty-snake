/**
 * engine.h
 *
 * tty-snake game engine.
 *
 * See LICENSE for copyright information.
 */

#ifndef ENGINE_H
#define ENGINE_H

#define ENGINE_TICKRATE   30 // max ticks per second
//#define TICKRATE_ENGINE   30 // max ticks per second
//#define TICKRATE_GAME     30
//#define TICKRATE_GRAPHICS 30

#define PAUSE_KEY 'p'
#define QUIT_KEY  'q'

#include <global.h>

extern bool is_engine_running;

void engine_start(void);
void engine_stop(void);

#endif // ENGINE_H
