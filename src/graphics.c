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
static int      old_curs;
static WINDOW * popup_win = NULL;

// private forward declarations
static void draw_titlebar(void);
static void draw_gs_paused(bool);
static void draw_gs_running(bool);
static void draw_gs_ending(bool);

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

    old_curs  = curs_set(0);

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
  // initially use illegal state so prev_game_state != game_state initially
  static enum gamestate_t prev_game_state = GS_COUNT;
  bool is_gamestate_change                = (prev_game_state != game_state);

  draw_titlebar();

  // close popup window if changing states
  if (is_gamestate_change)
  {
    nc_window_destroy(popup_win, true);
    popup_win = NULL;
  }

  // determine what to display based on game state
  switch (game_state)
  {
    // TODO - document
    case GS_STARTING:
      // TODO
      break;

    // draw game elements
    case GS_RUNNING:
      draw_gs_running(is_gamestate_change);
      break;

    // draw pause menu
    case GS_PAUSED:
      draw_gs_paused(is_gamestate_change);
      break;

    // draw post-game stats
    case GS_ENDING:
      draw_gs_ending(is_gamestate_change);
      break;  
  }

  prev_game_state = game_state;
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
    nc_window_destroy(popup_win, false);

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
  // re-draw top border
  mvhline(0, 1, ACS_HLINE, game_x_bound - 2);

  // TODO show powerup time remaining
  // draw gamestate string
  mvprintw(0, 2, "[ %s | SCORE: %d | POWERUP: %s ]",
    gamestate_to_string(game_state),
    game_score,
    powerup_to_string(snake->powerup)
  ); 
}


/*
 * Per-gamestate update functions
 */

/**
 * function:  draw_gs_running
 * --------------------------
 * TODO - Documentation
 */
static void draw_gs_paused(bool is_gamestate_change)
{
  if (is_gamestate_change)
  {
    int height = 6,
        width  = 50,
        starty = (LINES - height) / 2,
        startx = (COLS - width) / 2;

    // create the popup window
    popup_win = nc_window_create(height, width, starty, startx);

    // draw box around popup window
    box(popup_win, 0, 0);

    mvwprintw(popup_win, 1, 1, "GAME PAUSED");
    mvwprintw(popup_win, 2, 1, "PRESS P TO UNPAUSE");

    wrefresh(popup_win);

  }
}

/**
 * function:  draw_gs_running
 * --------------------------
 * TODO - Documentation
 */
static void draw_gs_running(bool is_gamestate_change)
{
  struct ent_snake_seg * dead_seg = snake->tail;

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
static void draw_gs_ending(bool is_gamestate_change)
{
  if (is_gamestate_change)
  {
    int height = 6,
        width  = 50,
        starty = (LINES - height) / 2,
        startx = (COLS - width) / 2;

    // create the popup window
    popup_win = nc_window_create(height, width, starty, startx);

    // draw box around popup window
    box(popup_win, 0, 0);

    mvwprintw(popup_win, 1, 1, "GAME OVER!");
    mvwprintw(popup_win, 2, 1, "Q TO QUIT");

    wrefresh(popup_win);
  }
}

