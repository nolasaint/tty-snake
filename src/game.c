/**
 * game.c
 *
 * tty-snake game module (provides game element definitions).
 *
 * See LICENSE for copyright information.
 */

#include <stdlib.h> // malloc(), rand()

#include <game.h>

// external global variables
bool is_game_over;
bool is_game_paused;
struct ent_food  * food;
struct ent_snake * snake;

// global variables
// TODO: this uses a lot of memory, testing component-based checks first
// static bool collision_map[][];
static unsigned int powerup_durations[PU_COUNT];

static void powerup_init(void)
{
  // initialize powerup durations
  powerup_durations[PU_SINGLESTEP] = PU_SINGLESTEP_DUR;
  powerup_durations[PU_NOGROW]     = PU_NOGROW_DUR;
}

/**
 * function: snake_set_powerup
 * ---------------------------
 * TODO: Documentation
 */
static void snake_set_powerup(enum powerup_t powerup)
{
  // TODO
  return;
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

  // TODO use configurable powerup spawning rarity
  // TODO use individual powerup rarities
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
         should_grow = false;   // if true, tail not marked dying
  int    dx = 0, dy = 0;        // change in x and y cooridnates
  struct ent_snake_seg * new_seg;

  // free dying segments and remove from snake
  while (snake->tail && snake->tail->dying)
  {
    struct ent_snake_seg * old_tail = snake->tail;

    snake->tail = snake->tail->prev;
    snake->tail->next = NULL;

    free(old_tail);
  }

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

    // check if allocation failed
    if (!new_seg)
      quit();

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

    // TODO check if powerup timer runs out (use powerup_durations array)

    // check if snake consumed food
    if (!food->consumed && ARE_COLLIDING(food, snake->head))
    {
      should_grow    = true;
      food->consumed = true;

      // TODO if snake has powerup, don't spawn food with powerup
      // TODO once this is done, we can remove the !snake->powerup check
      // don't override current powerup
      if (PU_NONE != food->powerup)
      {
        snake->powerup = food->powerup;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &snake->powerup_start_ts);
      }

      // TODO eventually use milliseconds food respawn countdown
      // XXX for now, immediately spawn new food
      food_spawn((bool) !snake->powerup);
    }

    // pop tail and mark dying if snake is not growing
    if (!should_grow || PU_NOGROW == snake->powerup)
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
      snake->velocity = VEL_NONE;

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
