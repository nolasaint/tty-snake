/**
 * graphics.c
 *
 * tty-snake graphics module.
 *
 * See LICENSE for copyright information.
 */

#include <ncurses.h>
#include <stdio.h>   // sprintf()

#include <game.h>

#include <graphics.h>

// external global variables
bool is_graphics_setup = false; // graphics.h

unsigned int game_x_bound; // game.h
unsigned int game_y_bound; // game.h
struct ent_food  * food;   // game.h
struct ent_snake * snake;  // game.h

// global variables
static int old_curs;

// private forward declarations
static void draw_titlebar(void);
static void draw_gs_running(void);
static void draw_gs_ending(void);

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
    cbreak();
    getmaxyx(stdscr, game_y_bound, game_x_bound);

    old_curs = curs_set(0);

    // draw game area boundary (use defaults since they look pretty)
    border(0,0,0,0,0,0,0,0);

    is_graphics_setup = true;
  }
}

/**
 * function:  graphics_update
 * --------------------------
 * updates the on-screen graphics.
 */
void graphics_update(void)
{
  // static enum gamestate_t prev_gamestate;

  // determine what to display based on game state
  switch (game_state)
  {
    // TODO - document
    case GS_STARTING:
      // TODO
      break;

    // draw game elements
    case GS_RUNNING:
      draw_gs_running();
      break;

    case GS_PAUSED:
      // TODO should probably still do draw_gameplay_screen
      break;

    // draw post-game stats
    case GS_ENDING:
      draw_gs_ending();
      break;  
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
    curs_set(old_curs);
    endwin();

    is_graphics_setup = false;
  }
}

/**
 * function:  draw_titlebar
 * ------------------------
 * TODO - Documentation
 */
static void draw_titlebar(void)
{
  
}

/*
 * Per-gamestate update functions
 */

/**
 * function:  draw_gs_running
 * --------------------------
 * TODO - Documentation
 * TODO - take in boolean to designate if gamestate change has occurred
 */
static void draw_gs_running(void)
{
  struct ent_snake_seg * dead_seg = snake->tail;

  // TODO: check if game is paused (can use menus for this)

  // re-draw top border
  mvhline(0, 1, ACS_HLINE, game_x_bound - 2);

  // TODO show powerup time remaining
  // draw gamestate string
  mvprintw(
    0, 2, "[ TTY-SNAKE | SCORE: %d | POWERUP: %s ]",
    game_score, powerup_get_name(snake->powerup)
  );

  // erase dead segments from screen
  while (dead_seg && dead_seg->dying)
  {
    // printw overwrites text, use it instead of delch()
    mvprintw(dead_seg->y, dead_seg->x, " ");
    dead_seg = dead_seg->prev;
  }

  // draw entities
  // TODO maybe check if either entity has moved? how?

  // overwrite previous head with body segment, if body segments exist
  if (snake->length > 2)
    mvaddch(snake->head->next->y, snake->head->next->x, ENT_SNAKE_DISP);

  // draw tail if it is not the head
  if (snake->length > 1)
    mvaddch(dead_seg->y, dead_seg->x, ENT_SNAKE_TAIL_DISP);

  // draw head
  mvaddch(snake->head->y, snake->head->x, ENT_SNAKE_HEAD_DISP);

  if (!food->consumed)
  {
    char food_display_char;

    // special display if food has powerup
    switch (food->powerup)
    {
      case PU_SINGLESTEP:
        // TODO
        food_display_char = 'S' | ENT_FOOD_ATTR;
        break;

      case PU_NOGROW:
        // TODO
        food_display_char = 'N' | ENT_FOOD_ATTR;
        break; 

      // PU_NONE, other non-valid states
      default:
        food_display_char = ENT_FOOD_CH | ENT_FOOD_ATTR;
        break;
    }

    mvaddch(food->y, food->x, food_display_char);
  }
}

/**
 * function:  draw_gs_ending
 * -------------------------
 * TODO - Documentation
 */
static void draw_gs_ending(void)
{
  // TODO do we even want this as it's own screen?
  mvprintw(0,0,"GAME OVER!");
  mvprintw(1,0,"PRESS ENTER OR Q TO EXIT");
}

