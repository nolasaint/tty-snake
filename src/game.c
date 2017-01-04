/**
 * game.c
 *
 * tty-snake game module (provides game element definitions).
 *
 * See COPYRIGHT for copyright information.
 */

#include <stdlib.h> // malloc()

#include <game.h>

// external global variables
struct ent_snake * snake;

/**
 * function:  game_setup
 * ---------------------
 * initializes game elements.
 *
 * init_x:  initial x coordinate for the snake
 * init_y:  initial y coordinate for the snake
 */
void game_setup(unsigned int init_x, unsigned int init_y)
{
  snake = calloc(1, sizeof(struct ent_snake));

  snake->head = malloc(sizeof(struct ent_snake_seg));
  snake->tail = malloc(sizeof(struct ent_snake_seg));

  *snake->head = (struct ent_snake_seg) {
    .dying = false,
    .x     = init_x,
    .y     = init_y,
    .prev  = NULL,
    .next  = NULL
  };

  // snake initially only one segment long TODO configurable?
  snake->tail   = snake->head;
  snake->length = 1;
}

// TODO - Documentation
bool game_update(void)
{
  // TODO
  // TODO delete dying segments
  // TODO update head
  // TODO mark tail segment(s) dying
}

// TODO - Documentation
void game_unset(void)
{
  struct ent_snake_seg * next_seg = snake->head;

  // TODO free food if it exists

  // free memory allocated with malloc
  while (next_seg)
  {
    struct ent_snake_seg * free_seg = next_seg;
    next_seg = next_seg->next;

    free(free_seg);
  }

  free(snake);
}
