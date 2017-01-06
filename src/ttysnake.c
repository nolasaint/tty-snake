/**
 * tty-snake
 *
 * a unix terminal-based 'snake' arcade game.
 *
 * 2 January 2017
 *
 * version 0.1
 */

#include <signal.h> // signal(), SIGTERM
#include <stdlib.h> // on_exit()
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
