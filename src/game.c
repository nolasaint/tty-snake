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
  bool         is_colliding = false,
               should_grow  = false; // if true, tail not marked dying
  int          dx = 0, dy = 0;       // change in x and y cooridnates
  nanosecond_t now_ns = get_time_ns();
  struct ent_snake_seg * new_seg;

  struct game_updatecycle_info uc_info = {
    .start_ns           = get_time_ns(),
    .snake_dx           = 0,
    .snake_dy           = 0,
    .snake_should_grow  = false,
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

  // TODO idea:
  // TODO powerup_enable()
  // TODO pass a game_state_info struct
  // TODO or game_update_info struct idc
  // TODO this will keep track of stuff like should_grow
  // TODO the powerup_enable() or powerup_tick() could modify it
  // TODO could also have a powerup_post_tick() if stuff needs to be done later

  // check if powerup timer runs out (use powerup_durations array)
  if (PU_NONE != snake->powerup)
  {
//    nanosecond_t alive_ns = now_ns - snake->powerup_start_ns;
    nanosecond_t alive_ns = uc_info.start_ns - snake->powerup_start_ns;

    if (alive_ns >= powerup_durations[snake->powerup])
    {
      // resume snake momentum if single-step powerup expires
      if (PU_SINGLESTEP == snake->powerup)
      {
        snake_set_velocity(snake->prev_velocity);

        // ensure snake doesn't return to VEL_NONE
        uc_info.snake_new_velocity = snake->velocity;
      }

      snake->powerup = PU_NONE;
    }
  }

  // TODO this is where the powerup check would go

  // update head if snake is moving
  if (snake->velocity != VEL_NONE)
  {
    switch (snake->velocity)
    {
      case VEL_UP:
//        dy = -1;
        uc_info.snake_dy = -1;
        break;

      case VEL_RIGHT:
//        dx = 1;
        uc_info.snake_dx = 1;
        break;

      case VEL_DOWN:
//        dy = 1;
        uc_info.snake_dy = 1;
        break;

     case VEL_LEFT:
//        dx = -1;
        uc_info.snake_dx = -1;
        break;
    }

    // set up new_seg
    new_seg = malloc(sizeof(struct ent_snake_seg));

    // check if allocation failed
    if (!new_seg)
      quit(); // TODO maybe use a diff function if quit() doesnt behave

    *new_seg = (struct ent_snake_seg) {
      .dying = false,
//      .x     = snake->head->x + dx,
//      .y     = snake->head->y + dy,
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
//      should_grow    = true;
      uc_info.snake_should_grow = true;
      food->consumed = true;

      // don't override current powerup
      if (PU_NONE != food->powerup)
      {
        snake->powerup = food->powerup;
        snake->powerup_start_ns = get_time_ns();
      }

      // TODO eventually use milliseconds food respawn countdown
      // don't allow powerups to spawn if one is already active
      food_spawn(PU_NONE == snake->powerup);
    }

    // pop tail and mark dying if snake is not growing
//    if (!should_grow || PU_NOGROW == snake->powerup)
    if (!uc_info.snake_should_grow || PU_NOGROW == snake->powerup)
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

    // TODO should powerup checks be separate from normal logic?

    // check if single-step powerup is active
    if (PU_SINGLESTEP == snake->powerup)
      snake_set_velocity(VEL_NONE);

    // TODO other ways to lose / win?
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
  // TODO move this somewhere else
  enum velocity_t opposite_velocity[5];

  opposite_velocity[VEL_NONE]  = VEL_NONE;
  opposite_velocity[VEL_UP]    = VEL_DOWN;
  opposite_velocity[VEL_RIGHT] = VEL_LEFT;
  opposite_velocity[VEL_DOWN]  = VEL_UP;
  opposite_velocity[VEL_LEFT]  = VEL_RIGHT;

  enum velocity_t illegal_velocity;

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

