/**
 * graphics.c
 *
 * tty-snake graphics module.
 *
 * See COPYRIGHT for copyright information.
 */

#include <ncurses.h>
#include <game.h>
#include <graphics.h>

// external global variables
_Bool is_graphics_setup = false; // graphics.h

/**
 * function:  graphics_setup
 * -------------------------
 * initializes the graphics module.
 */
void graphics_setup(void)
{
  if (!is_graphics_setup)
  {
    // ncurses setup
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    is_graphics_setup = true;
  }
}

/**
 * function:  graphics_unset
 * -------------------------
 * uninitializes the graphics module.
 */
void graphics_unset(void)
{
  if (is_graphics_setup)
  {
    // ncurses unset
    refresh();
    endwin();

    is_graphics_setup = false;
  }
}
