/**
 * tty-snake - version 0.1
 *
 * a unix terminal-based 'snake' arcade game.
 *
 * See LICENSE for copyright information.
 */

#include <signal.h> // signal(), SIGINT, SIGTERM
#include <stdlib.h> // on_exit()
#include <stdio.h>  // printf()
#include <time.h>   // time()

#include <engine.h> // engine_start(), engine_stop()

void sig_handler(int signum)
{
  engine_stop();
}

void exit_handler(int ev, void * arg)
{
  engine_stop();
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
  struct sigaction action;

  action.sa_handler = sig_handler;

  on_exit(exit_handler, NULL);

  sigaction(SIGINT,  &action, NULL);
  sigaction(SIGTERM, &action, NULL);

  srand(time(NULL));

  engine_start();
}
