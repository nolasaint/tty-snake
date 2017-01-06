/**
 * tty-snake - version 0.1
 *
 * a unix terminal-based 'snake' arcade game.
 *
 * See LICENSE for copyright information.
 */

#include <signal.h> // signal(), SIGTERM
#include <stdlib.h> // on_exit()
#include <stdio.h>  // printf()

#include <engine.h> // engine_start(), engine_stop()

void sig_handler(int signum)
{
  printf("sig_handler\n");
  engine_stop();
}

void exit_handler(int ev, void * arg)
{
  printf("exit_handler\n");
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
 *
 * TODO: use on_exit() to set up graceful exit handler (call engine_stop())
 * TODO: implement graceful exit handler: exit_handler()
 */
int main(int argc, char **argv)
{
  struct sigaction action;

  action.sa_handler = sig_handler;

  on_exit(exit_handler, NULL);
  sigaction(SIGTERM, &action, NULL);

  engine_start();
}
