/**
 * game.c
 *
 * tty-snake game module (provides game element definitions).
 *
 * See LICENSE for copyright information.
 */

#include <stdlib.h> // malloc()
#include <stdio.h>

#include <game.h>

// external global variables
struct ent_food  * food;
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
  food  = calloc(1, sizeof(struct ent_food));
  snake = calloc(1, sizeof(struct ent_snake));

  // XXX enable single step for right now
  snake->powerup = PU_SINGLESTEP;

  // we don't calloc since we have to initialize them anyway
  snake->head = malloc(sizeof(struct ent_snake_seg));
  snake->tail = malloc(sizeof(struct ent_snake_seg));

  // check if allocations failed
  if (!food || !snake || !snake->head || !snake->tail)
    quit();

  *snake->head = (struct ent_snake_seg) {
    .dying = false,
    .x     = init_x,
    .y     = init_y,
    .prev  = NULL,
    .next  = NULL
  };

  // snake initially only one segment long
  snake->tail   = snake->head;
  snake->length = 1;
}

/**
 * function:  game_update
 * ----------------------
 * TODO - Documentation
 */
bool game_update(void)
{
  bool should_grow = false; // if true, tail not marked dying
  int dx = 0, dy = 0;       // change in x and y cooridnates
  struct ent_snake_seg * new_seg;

  // free dying segments and remove from snake
  while (snake->tail && snake->tail->dying)
  {
    struct ent_snake_seg * old_tail = snake->tail;

    snake->tail = snake->tail->prev;
    snake->tail->next = NULL;

    free(old_tail);
  }

  // TODO check if snake->tail was null (shouldn't happen so log it

  // update head if snake is moving
  if (snake->velocity != VEL_NONE)
  {
    switch (snake->velocity)
    {
      case VEL_UP:
        dy = -1;
        break;

      case VEL_RIGHT:
        dx = 1;
        break;

      case VEL_DOWN:
        dy = 1;
        break;

     case VEL_LEFT:
        dx = -1;
        break;
    }

    // set up new_seg
    new_seg = malloc(sizeof(struct ent_snake_seg));

    // TODO check that malloc didn't fail
    *new_seg = (struct ent_snake_seg) {
      .dying = false,
      .x     = snake->head->x + dx,
      .y     = snake->head->y + dy,
      .prev  = NULL,
      .next  = snake->head
    };

    snake->head->prev = new_seg;
    snake->head = new_seg;
    snake->length++;

    // check if snake consumed food (free(food) if so, or return to pool)
    if (food && food->x == snake->head->x && food->y == snake->head->y)
    {
      should_grow = true;
      snake->powerup = food->powerup;

      free(food);
    }

    // pop tail and mark dying if snake is not growing
    if (!should_grow && PU_NOGROW != snake->powerup)
    {
      snake->tail->dying = true;
      snake->length--;
    }

    // TODO should powerup checks be separate from normal logic?

    // check if single-step powerup is active
    if (PU_SINGLESTEP == snake->powerup)
      snake->velocity = VEL_NONE;

    // TODO check if game over (run into wall or tail)
  }

  return true;
}

/**
 * function:  game_unset
 * ---------------------
 * TODO - Documentation
 */
void game_unset(void)
{
  struct ent_snake_seg * next_seg = snake->head;

  // free food if it exists
  if (food)
    free(food);

  // free memory allocated with malloc
  while (next_seg)
  {
    struct ent_snake_seg * free_seg = next_seg;
    next_seg = next_seg->next;

    free(free_seg);
  }

  free(snake);
}
