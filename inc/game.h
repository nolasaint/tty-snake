/**
 * game.h
 *
 * tty-snake game module (provides game element definitions).
 *
 * See LICENSE for copyright information.
 */

#ifndef GAME_H
#define GAME_H

#define ARE_COLLIDING(ent_a,ent_b) \
  ((ent_a)->x == (ent_b)->x && (ent_a)->y == (ent_b)->y)

// snake display settings
#define ENT_SNAKE_HEAD_CH   'X'
#define ENT_SNAKE_HEAD_ATTR A_BOLD | A_STANDOUT | A_BLINK

// food display settings
#define ENT_FOOD_CH   'O' //'•'
#define ENT_FOOD_ATTR A_NORMAL

// powerup durations (in seconds)
#define PU_SINGLESTEP_DUR 30
#define PU_NOGROW_DUR     30

#include <global.h>

/**
 * enum:  velocity_t
 * -----------------
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
 * enum:  powerup_t
 * ----------------
 * TODO - Documentation
 */
enum powerup_t
{
  PU_NONE = 0,
  PU_SINGLESTEP, //
  PU_NOGROW,     //
};

/**
 * struct:  ent_food
 * -----------------
 * TODO - Documentation
 */
struct ent_food
{
  bool consumed;

  unsigned int x;
  unsigned int y;

  enum powerup_t powerup;
};

/**
 * struct:  ent_snake
 * ------------------
 * TODO - Documentation
 */
struct ent_snake
{
  unsigned int length;
  // TODO powerup start timestamp

  enum velocity_t velocity;
  enum powerup_t  powerup;

  struct ent_snake_seg * head;
  struct ent_snake_seg * tail;
};

/**
 * struct:  ent_snake_seg
 * ----------------------
 * TODO - Documentation
 */
struct ent_snake_seg
{
  bool dying;

  unsigned int x;
  unsigned int y;

  struct ent_snake_seg * prev; // NULL if head
  struct ent_snake_seg * next; // NULL if tail
};

// Game area bounds
extern unsigned int game_x_bound;
extern unsigned int game_y_bound;

// Entities TODO: const?
extern struct ent_food  * food;
extern struct ent_snake * snake;

void game_setup(unsigned int init_x, unsigned int init_y);
bool game_update(void);
void game_unset(void);

#endif // GAME_H
