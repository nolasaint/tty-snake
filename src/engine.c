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
bool is_engine_running;  // engine.h

unsigned int game_x_bound; // game.h
unsigned int game_y_bound; // game.h
struct ent_snake * snake;  // game.h

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
    last_ch = getch();

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

/**
 * function:  engine_start
 * -----------------------
 * starts the game engine.
 */
void engine_start(void)
{
  const struct timespec ZERO_TS = {0};
  const struct timespec MAX_ELAPSED_TS = {
    .tv_sec  = MS2S(ENGINE_MS_PER_TICK),
    .tv_nsec = MS2NS(ENGINE_MS_PER_TICK) - MS2NS(S2MS(MS2S(ENGINE_MS_PER_TICK)))
  };

  #ifdef DEBUG
  //fprintf(stderr, "EFAULT = %d\nEINVAL = %d\nEPERM = %d\n", EFAULT,EINVAL,EPERM);
  fprintf(stderr, "MAX_ELAPSED_TS is %d ms\n", TIMESPEC2MS(MAX_ELAPSED_TS));
  #endif

  is_engine_running = true;
  do_tick           = true;

  // setup modules
  graphics_setup(); // does ncurses initialization
  game_setup(game_x_bound / 2, game_y_bound / 2);

  // set timeout so getch() is non-blocking
  timeout(0);

#ifdef USE_KB_LISTEN_THREAD
  // start keyboard listening thread
  pthread_create(&kb_listen_threadid, NULL, kb_listen, NULL);
#endif

  // engine tick
  while (do_tick)
  {
    struct timespec elapsed_ts,
                    start_ts,
                    end_ts;

    // reset process clock to avoid overflow / make math easier
//    int retval = clock_settime(CLOCK_PROCESS_CPUTIME_ID, &ZERO_TS);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_ts);

//    #ifdef DEBUG
//    fprintf(stderr, "clock_settime returned %d\n", retval);
//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &elapsed_ts);
//    fprintf(stderr, "CLOCK_PROCESS_CPUTIME_ID is %d ms approx\n", TIMESPEC2MS(elapsed_ts));
//    #endif

    // check for keyboard input
#ifdef USE_KB_LISTEN_THREAD
    switch (last_ch)
#else
    switch (getch())
#endif
    {
      case KEY_UP:
      case 'w':
        snake->velocity = VEL_UP;
        break;

      case KEY_RIGHT:
      case 'd':
        snake->velocity = VEL_RIGHT;
        break;

      case KEY_DOWN:
      case 's':
        snake->velocity = VEL_DOWN;
        break;

      case KEY_LEFT:
      case 'a':
        snake->velocity = VEL_LEFT;
        break;

      // ignore getch() returning from timeout()
      case ERR:
        break;

      // quit immediately
      case 'q':
        goto quit;
    }

#ifdef USE_KB_LISTEN_THREAD
    last_ch = -1;
#endif

    // TODO use update rates for game_update and graphics_update

    // update entities and re-draw
    game_update(); 
    graphics_update();

//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &elapsed_ts);
//
//    #ifdef DEBUG
//    fprintf(stderr, "%d ms elapsed\n", TIMESPEC2MS(elapsed_ts));
//    #endif

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_ts);

    int max_ms   = TIMESPEC2MS(MAX_ELAPSED_TS),
        start_ms = TIMESPEC2MS(start_ts),
        end_ms   = TIMESPEC2MS(end_ts);

    // sleep if less than ENGINE_MS_PER_TICK ms have elapsed
//    if (TIMESPEC2MS(elapsed_ts) < ENGINE_MS_PER_TICK)
    if (end_ms - start_ms < ENGINE_MS_PER_TICK)
    {
      // use elapsed_tp to store time we want to sleep for
//      ms2timespec(TIMESPEC2MS(MAX_ELAPSED_TS) - TIMESPEC2MS(elapsed_ts), &elapsed_ts);
//      #ifdef DEBUG
//      fprintf(stderr, "sleeping for %d ms\n", TIMESPEC2MS(elapsed_ts));
//      #endif
//      nanosleep(&elapsed_ts, NULL);

      ms2timespec(max_ms - (end_ms - start_ms), &end_ts);
      nanosleep(&end_ts, NULL);
    }
  }

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
