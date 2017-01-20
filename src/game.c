/**
 * game.c
 *
 * tty-snake game module (provides game element definitions).
 *
 * See LICENSE for copyright information.
 */

#include <stdlib.h> // malloc(), rand()

#include <game.h>

#include <ncurses.h>

// external global variables
bool is_game_over;
bool is_game_paused;
struct ent_food  * food;
struct ent_snake * snake;

// global variables
// TODO: this uses a lot of memory, testing component-based checks first
// static bool collision_map[][];
static nanosecond_t powerup_durations[PU_COUNT];

/**
 * function: powerup_init
 * ----------------------
 * TODO: Documentation
 */
static void powerup_init(void)
{
  // initialize powerup durations
  powerup_durations[PU_SINGLESTEP] = (nanosecond_t) PU_SINGLESTEP_DUR * SECONDS;
  powerup_durations[PU_NOGROW]     = (nanosecond_t) PU_NOGROW_DUR * SECONDS;
}

/**
 * function:  powerup_tick
 * -----------------------
 * TODO - Documentation
 *
 * When enabling a newly-acquired (aka just consumed) powerup, check_expiry
 * should be set to false so no time is wasted checking if the powerup is
 * expired.
 *
 * p_uc_info:     current update cycle's info struct
 * check_expiry:  if true, check if the powerup should expire
 */
static void powerup_tick(
    struct game_updatecycle_info * p_uc_info,
    bool   check_expiry
)
{
  // return immediately if no powerup is active
  if (PU_NONE == snake->powerup)
    return;

  // check if powerup has expired
  if (check_expiry)
  {
    if (p_uc_info->start_ns >= snake->powerup_expire_ns)
    {
      // resume snake momentum if single-step powerup expires
      if (PU_SINGLESTEP == snake->powerup)
      {
        snake_set_velocity(snake->prev_velocity);

        // ensure snake doesn't return to VEL_NONE
        p_uc_info->snake_new_velocity = snake->velocity;
     }

      snake->powerup = PU_NONE;
    }
  }

  // set update cycle information based on powerup
  switch (snake->powerup)
  {
    // no powerup
    case PU_NONE:
    case PU_COUNT:
      break;

    // snake moves one unit at a time with this powerup
    case PU_SINGLESTEP:
      p_uc_info->snake_new_velocity = VEL_NONE;
      break;

    // snake does not grow when consuming food with this powerup
    case PU_NOGROW:
      p_uc_info->snake_can_grow = false;
      break;
  }
}

/*
 * function:  powerup_activate
 * ---------------------------
 * TODO - Documentation
 */
static void powerup_activate(
    enum   powerup_t               powerup,
    struct game_updatecycle_info * p_uc_info
)
{
  snake->powerup           = powerup;
  snake->powerup_expire_ns = p_uc_info->start_ns + powerup_durations[powerup];

  // don't waste time checking expiry for newly-acquired powerup
  powerup_tick(p_uc_info, false);
}

/**
 * function: rand_powerup
 * ----------------------
 * returns a random powerup_t based on pre-defined probabilities.
 *
 * returns: a randomly-selected powerup
 */
static enum powerup_t rand_powerup(void)
{
  // TODO use probabilities
  return (enum powerup_t)(rand() % PU_COUNT);
}

/**
 * function:  food_spawn
 * ---------------------
 * randomly place a food bit (potentially with powerup) on the board.
 *
 * allow_powerup: true if food can spawn with a powerup
 */
static void food_spawn(bool allow_powerup)
{
  unsigned int rand_x, rand_y;

  do
  {
    // rand seeded in ttysnake.c:main
    rand_x = rand() % game_x_bound;
    rand_y = rand() % game_y_bound;
  } while (rand_x == snake->head->x && rand_y == snake->head->y);

  food->powerup = PU_NONE;

  // rarely, spawn powerup (if allowed)
  if (allow_powerup && (rand() % 100) <= PU_SPAWN_PERCENTAGE)
    food->powerup = rand_powerup();

  food->x = rand_x;
  food->y = rand_y;

  food->consumed = false;
}

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
  // call other initialization functions
  powerup_init();

  is_game_over = false;

  food  = calloc(1, sizeof(struct ent_food));
  snake = calloc(1, sizeof(struct ent_snake));

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

  // randomly place initial food piece
  food_spawn(false);

  // snake initially only one segment long
  snake->tail    = snake->head;
  snake->length  = 1;

  snake->powerup = PU_NONE;
}

/**
 * function:  game_update
 * ----------------------
 * TODO - Documentation
 */
bool game_update(void)
{
  bool   is_colliding = false,
         should_grow  = false;
  struct ent_snake_seg * new_seg;

  struct game_updatecycle_info uc_info = {
    .start_ns           = get_time_ns(),
    .snake_dx           = 0,
    .snake_dy           = 0,
    .snake_can_grow     = true,
    .snake_should_move  = true,
    .snake_new_velocity = snake->velocity // initially unchanging
  };

  // free dying segments and remove from snake
  while (snake->tail && snake->tail->dying)
  {
    struct ent_snake_seg * old_tail = snake->tail;

    snake->tail = snake->tail->prev;
    snake->tail->next = NULL;

    free(old_tail);
  }

  // update uc_info based on powerup, if one is active
  powerup_tick(&uc_info, true);

  // update head if snake is moving
  if (snake->velocity != VEL_NONE)
  {
    // update snake's x or y coordinate based on velocity, if allowed
    if (uc_info.snake_should_move)
    {
      switch (snake->velocity)
      {
        case VEL_UP:
          uc_info.snake_dy = -1;
          break;

        case VEL_RIGHT:
         uc_info.snake_dx = 1;
          break;

        case VEL_DOWN:
          uc_info.snake_dy = 1;
          break;

       case VEL_LEFT:
          uc_info.snake_dx = -1;
          break;
      }
    }

    // set up new_seg
    new_seg = malloc(sizeof(struct ent_snake_seg));

    // check if allocation failed
    if (!new_seg)
      quit(); // TODO maybe use a diff function if quit() doesnt behave

    *new_seg = (struct ent_snake_seg) {
      .dying = false,
      .x = snake->head->x + uc_info.snake_dx,
      .y = snake->head->y + uc_info.snake_dy,
      .prev  = NULL,
      .next  = snake->head
    };

    snake->head->prev = new_seg;
    snake->head = new_seg;
    snake->length++;

    // check if snake consumed food
    if (!food->consumed && ARE_COLLIDING(food, snake->head))
    {
      should_grow    = true;
      food->consumed = true;

      // absorb food's powerup
      if (PU_NONE != food->powerup)
        powerup_activate(food->powerup, &uc_info);

      // don't allow powerups to spawn if one is already active
      food_spawn(PU_NONE == snake->powerup);
    }

    // pop tail and mark dying if snake is not growing
    if (!uc_info.snake_can_grow || !should_grow)
    {
      snake->tail->dying = true;
      snake->length--;
    }

    // collision detection: ent_snake segments
    if (snake->length > 1)
    {
      struct ent_snake_seg * check_seg = snake->head->next;

      while (check_seg && !check_seg->dying)
      {
        if (ARE_COLLIDING(snake->head, check_seg))
        {
          is_colliding = true;
          break;
        }

        check_seg = check_seg->next;
      }
    }

    // update snake velocity (usually due to powerups)
    snake_set_velocity(uc_info.snake_new_velocity);

    // TODO other ways to lose / win?
    // TODO game over: win or lose? can you only lose?

    // check if game is over
    is_game_over = is_colliding;
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

/**
 * function:  snake_set_velocity
 * -----------------------------
 * TODO - documetation
 */
void snake_set_velocity(enum velocity_t velocity)
{
  enum velocity_t opposite_velocity[5];
  enum velocity_t illegal_velocity;

  opposite_velocity[VEL_NONE]  = VEL_NONE;
  opposite_velocity[VEL_UP]    = VEL_DOWN;
  opposite_velocity[VEL_RIGHT] = VEL_LEFT;
  opposite_velocity[VEL_DOWN]  = VEL_UP;
  opposite_velocity[VEL_LEFT]  = VEL_RIGHT;

  // TODO if snake is only one unit long, is backtracking OK?

  // disallow backtracking (180 deg velocity change)
  switch (snake->velocity)
  {
    // likely in single-step mode, so check prev_velocity
    case VEL_NONE:
      illegal_velocity = opposite_velocity[snake->prev_velocity];
      break;

    // all other times, check current velocity
    default:
      illegal_velocity = opposite_velocity[snake->velocity];
      break;
  }

  // don't update velocity if unchanging or illegal
  if (velocity != illegal_velocity && velocity != snake->velocity)
  {
    snake->prev_velocity = snake->velocity;
    snake->velocity      = velocity;
  }
}

