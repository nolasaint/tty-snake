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

// average # of powerups per 100 food spawns
#define PU_SPAWN_PERCENTAGE 10

// powerup durations (in seconds)
#define PU_SINGLESTEP_DUR 10
#define PU_NOGROW_DUR     15

#include <global.h>

/**
 * enum:  gamestate_t
 * ------------------
 * TODO - Documentation
 *
 * GS_STARTING: game is not yet started (title screen) TODO
 * GS_RUNNING:  game is running (being played)
 * GS_PAUSED:   game is paused (but not over) TODO
 * GS_ENDING:   game is over
 * GS_COUNT:    number of game states
 */
enum gamestate_t
{
  GS_STARTING = 0,
  GS_RUNNING,
  GS_PAUSED,
  GS_ENDING,
  GS_COUNT
};

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
  PU_NONE = -1,
  PU_SINGLESTEP,
  PU_NOGROW,
  PU_COUNT       // Simply stores the number of powerup_t items
}; // TODO let PU_NONE be 0, and PU_COUNT somehow be defined?

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
  nanosecond_t powerup_expire_ns;

  enum velocity_t velocity;
  enum velocity_t prev_velocity;
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

/**
 * struct:  game_updatecycle_info
 * ------------------------------
 * start_ns:  the nanosecond at which this update cycle began
 *
 * snake_dx:            the change in the snake's x coordinate
 * snake_dy:            the change in the snake's y coordinate
 * snake_can_grow:      whether the snake grows on food consumption
 * snake_should_move:   whether the snake should move automatically,
 *                        based on its velocity (usually set to false
 *                        if powerup is altering movement).
 * snake_new_velocity:  snake's new velocity at end of update cycle
 */
struct game_updatecycle_info
{
  // update cycle information
  nanosecond_t start_ns;

  // snake information
  int             snake_dx;
  int             snake_dy;
  bool            snake_can_grow;
  bool            snake_should_move;
  enum velocity_t snake_new_velocity;
};

// game status
extern enum gamestate_t game_state;
extern unsigned int     game_score;

// game area bounds
extern unsigned int game_x_bound;
extern unsigned int game_y_bound;

// entities
extern struct ent_food  * food;
extern struct ent_snake * snake;

// function declarations
void game_setup(unsigned int init_x, unsigned int init_y);
bool game_update(void);
void game_unset(void);

void snake_set_velocity(enum velocity_t velocity);

const char * gamestate_to_string(enum gamestate_t gamestate);
//const char * velocity_to_string(enum velocity_t velocity);
const char * powerup_to_string(enum powerup_t powerup);

#endif // GAME_H
