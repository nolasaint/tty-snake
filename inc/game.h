/**
 * game.h
 *
 * tty-snake game module (provides game element definitions).
 *
 * See COPYRIGHT for copyright information.
 */

#ifndef GAME_H
#define GAME_H

#include <global.h>

/**
 * TODO - Documentation
 */
enum velocity_t
{
  VEL_NONE = 0,
  VEL_UP,
  VEL_RIGHT,
  VEL_DOWN,
  VEL_LEFT
};

/**
 * TODO - Documentation
 */
struct ent_food
{
  unsigned int x;
  unsigned int y;
};

/**
 * TODO - Documentation
 */
struct ent_snake
{
  unsigned int length;

  enum velocity_t velocity;

  struct ent_snake_seg * head;
  struct ent_snake_seg * tail;
};

/**
 * TODO - Documentation
 */
struct ent_snake_seg
{
  bool dying;

  unsigned int x;
  unsigned int y;

  struct ent_snake_seg * prev;
  struct ent_snake_seg * next;
};

// Game area bounds
extern unsigned int game_x_bound;
extern unsigned int game_y_bound;

// Player entity
extern struct ent_snake * snake; // TODO: Const?

void game_setup(unsigned int init_x, unsigned int init_y);
bool game_update(void);
void game_unset(void);

#endif // GAME_H
