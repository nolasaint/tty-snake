/**
 * graphics.h
 *
 * tty-snake graphics module.
 *
 * See LICENSE for copyright information.
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <global.h>

extern bool is_graphics_setup;

void graphics_setup(void);
void graphics_update(void);
void graphics_unset(void);

#endif // GRAPHICS_H
