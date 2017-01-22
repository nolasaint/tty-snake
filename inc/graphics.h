/**
 * graphics.h
 *
 * tty-snake graphics module.
 *
 * See LICENSE for copyright information.
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

// snake display settings
#define ENT_SNAKE_CH        ' '
#define ENT_SNAKE_ATTR      A_BOLD | A_STANDOUT
#define ENT_SNAKE_DISP      ENT_SNAKE_CH | ENT_SNAKE_ATTR

#define ENT_SNAKE_HEAD_CH   'H'
#define ENT_SNAKE_HEAD_ATTR A_BOLD | A_STANDOUT
#define ENT_SNAKE_HEAD_DISP ENT_SNAKE_HEAD_CH | ENT_SNAKE_HEAD_ATTR

#define ENT_SNAKE_TAIL_CH   'T'
#define ENT_SNAKE_TAIL_ATTR A_BOLD | A_STANDOUT
#define ENT_SNAKE_TAIL_DISP ENT_SNAKE_TAIL_CH | ENT_SNAKE_TAIL_ATTR

// food display settings
#define ENT_FOOD_CH         'O' //'•'
#define ENT_FOOD_ATTR       A_NORMAL
#define ENT_FOOD_DISP       ENT_FOOD_CH | ENT_FOOD_ATTR

// popup window dimensions
#define WIN_PAUSE_HEIGHT  6
#define WIN_PAUSE_WIDTH   50

#define WIN_GAMEOVER_HEIGHT 6
#define WIN_GAMEOVER_WIDTH  50

#include <global.h>

extern bool is_graphics_setup;

void graphics_setup(void);
void graphics_update(void);
void graphics_unset(void);

#endif // GRAPHICS_H
