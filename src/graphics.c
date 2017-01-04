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
bool is_graphics_setup = false; // graphics.h

unsigned int game_x_bound; // game.h
unsigned int game_y_bound; // game.h

// global variables
static int old_curs;

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
    getmaxyx(stdscr, game_y_bound, game_x_bound);

    old_curs = curs_set(0);
    is_graphics_setup = true;
  }
}

// TODO - Documentation
void graphics_update(void)
{
  // TODO: check game state (game over? powerups?)
  // TODO: erase dying segments from tail
  // TODO:   start at tail, erase if dying, move to tail->prev, repeat
  // TODO: draw new head
  // TODO: draw new food if necessary
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
    curs_set(old_curs);
    endwin();

    is_graphics_setup = false;
  }
}
