/**
 * engine.c
 *
 * tty-snake game engine.
 *
 * See LICENSE for copyright information.
 */

#ifdef DEBUG
#include <stdio.h>
#endif

#include <ncurses.h> // getch()
#include <pthread.h> // pthread_create()

#include <game.h>
#include <graphics.h>

#include <engine.h>

// external global variables
bool is_engine_running;     // engine.h

unsigned int game_x_bound;  // game.h
unsigned int game_y_bound;
struct ent_snake * snake;

// global variables
static bool do_tick; // whether the engine should keep running

#ifdef USE_KB_LISTEN_THREAD
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
  {
    last_ch = getch();

    #ifdef DEBUG
    fprintf(stderr, "[kb_listen] last_ch = %d\n", last_ch);
    #endif
  }

  #ifdef DEBUG
  fprintf(stderr, "[kb_listen] exiting\n");
  #endif

  pthread_exit(NULL);
}
#endif // USE_KB_LISTEN_THREAD

/**
 * function:  _engine_stop
 * -----------------------
 * stops the engine normally.
 *
 * TODO could just do this at end of engine_start
 */
static void _engine_stop(void)
{
  // unset modules
  graphics_unset();
  game_unset();

#ifdef USE_KB_LISTEN_THREAD
  // signal and wait for kb listen thread to stop
  do_kb_listen = false;

  pthread_join(kb_listen_threadid, NULL);
#endif

  is_engine_running = false;
}

//static inline void tick(void)
//{
//
//}

/**
 * function:  engine_start
 * -----------------------
 * starts the game engine.
 */
void engine_start(void)
{
  // convert (ticks per second) to (ns per tick)
  const uint64_t MAX_ELAPSED_NS = (1 / (float)ENGINE_TICKRATE) * SECONDS;

  is_engine_running = true;
  do_tick           = true;

  // setup modules
  graphics_setup(); // does ncurses initialization
  game_setup(game_x_bound / 2, game_y_bound / 2);

#ifdef USE_KB_LISTEN_THREAD
  // start keyboard listening thread
  pthread_create(&kb_listen_threadid, NULL, kb_listen, NULL);
#else
  // set timeout so getch() is non-blocking
  timeout(0);
#endif

  // TODO maybe we should just break on is_game_over
  // engine tick
  while (do_tick) //&& !is_game_over)
  {
    nanosecond_t start_ns, end_ns, elapsed_ns;

    start_ns = get_time_ns();

    // check for keyboard input
#ifdef USE_KB_LISTEN_THREAD
    switch (last_ch)
#else
    switch (getch())
#endif
    {
      case KEY_UP:
      case 'w':
        snake_set_velocity(VEL_UP);
        break;

      case KEY_RIGHT:
      case 'd':
        snake_set_velocity(VEL_RIGHT);
        break;

      case KEY_DOWN:
      case 's':
        snake_set_velocity(VEL_DOWN);
        break;

      case KEY_LEFT:
      case 'a':
        snake_set_velocity(VEL_LEFT);
        break;

      // ignore getch() returning from timeout()
      case ERR:
        break;

      case PAUSE_KEY:
        // TODO use gamestate_set()
        if (GS_RUNNING == game_state)
          game_state = GS_PAUSED;
        else if (GS_PAUSED == game_state)
          game_state = GS_RUNNING;
        break;

      // select an item (menu, etc)
      case KEY_ENTER:
        if (GS_ENDING == game_state)
          goto quit;

      // quit immediately
      case QUIT_KEY:
        goto quit;
    }

#ifdef USE_KB_LISTEN_THREAD
    last_ch = -1;
#endif

    // update entities and re-draw
    if (GS_ENDING != game_state)
      game_update();

    graphics_update();

    // limit engine tickrate
    end_ns     = get_time_ns();
    elapsed_ns = end_ns - start_ns;

    // sleep if less than MAX_ELAPSED_NS ns have elapsed
    if (elapsed_ns < MAX_ELAPSED_NS)
    {
      struct timespec sleep_ts;

      ns2timespec(MAX_ELAPSED_NS - elapsed_ns, &sleep_ts);
      nanosleep(&sleep_ts, NULL);
    }
  } // end of tick loop

quit:
  _engine_stop();
}

/**
 * function:  engine_stop
 * ----------------------
 * stops the game engine.
 */
void engine_stop(void)
{
  // signal the engine to stop and wait for normal termination
  do_tick = false;
}
