/**
 * engine.c
 *
 * tty-snake game engine.
 *
 * See COPYRIGHT for copyright information.
 */

#include <ncurses.h> // getch()
#include <pthread.h> // pthread_create()

#include <game.h>
#include <graphics.h>

#include <engine.h>

// external global variables
bool is_engine_running;  // engine.h

unsigned int game_x_bound; // game.h
unsigned int game_y_bound; // game.h
struct ent_snake * snake;  // game.h

// global variables
static bool do_tick; // whether the engine should keep running
static int  last_ch; // most recently input key

// kb_listen thread variables
static bool      do_kb_listen = true; // 
static pthread_t kb_listen_threadid;  // id from pthread_create

/**
 * function:  kb_listen
 * --------------------
 * actively listens for keyboard input while updating last_ch.
 *
 * arg: unused argument (required by pthread_create)
 *
 * returns: NULL        (required by pthread_create)
 */
static void * kb_listen(void * arg)
{
  while (do_kb_listen)
    last_ch = getch();
}

/**
 * function:  start
 * ----------------
 * starts the game engine.
 */
void start(void)
{
  is_engine_running = true;
  do_tick           = true;

  // setup modules
  graphics_setup(); // does ncurses initialization
  game_setup(game_x_bound / 2, game_y_bound / 2);

  // start keyboard listening thread
  pthread_create(&kb_listen_threadid, NULL, kb_listen, NULL);

  // engine tick
  while (do_tick)
  {
    switch (last_ch)
    {
      case KEY_UP:
        snake->velocity = VEL_UP;
        break;

      case KEY_RIGHT:
        snake->velocity = VEL_RIGHT;
        break;

      case KEY_DOWN:
        snake->velocity = VEL_DOWN;
        break;

      case KEY_LEFT:
        snake->velocity = VEL_LEFT;
        break;

      case 'q':
        do_tick = false;
        break;
    }

    last_ch = -1;

    // XXX stop engine on quit (temp)
    if (!do_tick) break;

    // TODO this should probably happen in _Bool game_update
    switch (snake->velocity)
    {
      case VEL_UP:
        snake->head->y -= 1;
        break;

      case VEL_RIGHT:
        snake->head->x += 1;
        break;

      case VEL_DOWN:
        snake->head->y += 1;
        break;

      case VEL_LEFT:
        snake->head->x -= 1;
        break;
    }

    // TODO single-step mode
    // TODO this is an idea, "powerups" that allow stuff like this
    // TODO     (single step for X seconds or X movements)
    snake->velocity = 0;

    // XXX show cursor position (temp)
    mvprintw(snake->head->y, snake->head->x, "X");

    // TODO update snake position
    // TODO graphics_update
  }

  // unset modules
  graphics_unset();
  // TODO game_unset();
}

/**
 * function:  stop
 * ---------------
 * stops the game engine.
 */
void stop(void)
{
  // TODO
  // TODO if game engine is running in its own thread:
  // TODO   check if current thread is that thread
  // TODO   if not, kill that thread
  //is_engine_running = false;
}
