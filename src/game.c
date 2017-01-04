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

// TODO - Documentation
void game_setup(unsigned int init_x, unsigned int init_y)
{
  snake = calloc(1, sizeof(struct ent_snake));

  snake->head = malloc(sizeof(struct ent_snake_seg));
  snake->tail =  malloc(sizeof(struct ent_snake_seg));

  *snake->head = (struct ent_snake_seg) {
    .x    = init_x,
    .y    = init_y,
    .prev = NULL,
    .next = NULL
  };

  // snake initially only one segment long TODO configurable?
  snake->tail   = snake->head;
  snake->length = 1;
}
