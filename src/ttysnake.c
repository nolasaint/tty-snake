/**
 * tty-snake - version 0.1
 *
 * a unix terminal-based 'snake' arcade game.
 *
 * See LICENSE for copyright information.
 */

#include <assert.h> // assert()
#include <signal.h> // signal(), SIGINT, SIGTERM
#include <stdlib.h> // on_exit()
#include <stdio.h>  // printf()
#include <time.h>   // time()

#include <engine.h> // engine_start(), engine_stop()

static void test_timespec_conversions(void)
{
  const struct timespec GOOD_TS = {
    .tv_sec  = 1,
    .tv_nsec = 5
  };
  const uint64_t GOOD_NS = 1000000005;

  struct timespec test_ts;
  uint64_t test_ns;

  // test ns -> timespec (equal case)
  ns2timespec(GOOD_NS, &test_ts);
  assert(ARE_TIMESPEC_EQUAL(GOOD_TS, test_ts));

  // test ns -> timespec (non-equal case)
  ns2timespec(GOOD_NS + 5, &test_ts);
  assert(!ARE_TIMESPEC_EQUAL(GOOD_TS, test_ts));

  // test timespec -> ns (equal case)
  test_ns = TIMESPEC2NS(GOOD_TS);
  assert(GOOD_NS == test_ns);

  // test timespec -> ns (non-equal case)
  test_ts = (struct timespec) {
    .tv_sec  = 1,
    .tv_nsec = 6
  };
  test_ns = TIMESPEC2NS(test_ts);
  assert(GOOD_NS != test_ns);
}

void sig_handler(int signum)
{
  engine_stop();
}

void exit_handler(int ev, void * arg)
{
  engine_stop();
}

void setup_handlers(void)
{
  struct sigaction action = { .sa_handler = sig_handler };

  // handle in-process exit() calls
  on_exit(exit_handler, NULL);

  // handle SIGINT, SIGTERM signals
  sigaction(SIGINT,  &action, NULL);
  sigaction(SIGTERM, &action, NULL);
}

/**
 * function:  main
 * ---------------
 * tty-snake entry-point.
 *
 * argc:  number of arguments
 * argv:  argument array
 *
 * returns: 0 on success, else 1.
 */
int main(int argc, char **argv)
{
  // configure interrupt handlers
  setup_handlers();

  // test assertions
  test_timespec_conversions();

  // seed the randomizer
  srand(time(NULL));

  engine_start();
}
