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
enum gamestate_t game_state;
unsigned int     game_score;
struct ent_food  * food;
struct ent_snake * snake;

// global variables
static nanosecond_t powerup_durations[PU_COUNT];
static nanosecond_t gs_begin_ns; // when the current gamestate began
static unsigned int tick_count;

// private forward declarations
static void food_spawn(bool);

static bool gamestate_can_transition(enum gamestate_t, enum gamestate_t);

static enum powerup_t rand_powerup(void);
static void powerup_init(void);
static void powerup_tick(struct game_updatecycle_info *, bool);
static void powerup_activate(struct game_updatecycle_info *, enum powerup_t);


/*
 * game functions
 */

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

  tick_count  = 0;
  gs_begin_ns = get_time_ns();
  game_state  = GS_STARTING;
  game_score  = 0;

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

  tick_count++;

  // free dying segments and remove from snake
  while (snake->tail && snake->tail->dying)
  {
    struct ent_snake_seg * old_tail = snake->tail;

    snake->tail = snake->tail->prev;
    snake->tail->next = NULL;

    free(old_tail);
  }

  if (GS_RUNNING == game_state)
  {
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
          powerup_activate(&uc_info, food->powerup);

        // XXX for now, score updates whenever food is consumed
        game_score += 1 + snake->length;

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

      // collision detection: walls
      if (!is_colliding)
      {
        is_colliding = (
          snake->head->x <= 0 || snake->head->x >= game_x_bound - 1
          || snake->head->y <= 0 || snake->head->y >= game_y_bound - 1
        );
      }

      // update snake velocity (usually due to powerups)
      snake_set_velocity(uc_info.snake_new_velocity);

      // TODO other ways to lose / win?
      // TODO game over: win or lose? can you only lose?
      // check if game is over
      if (is_colliding)
        game_state = GS_ENDING;
    }
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


/*
 * per-gamestate update functions
 */

// TODO


/*
 * food functions
 */

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
    rand_x = rand() % (game_x_bound - 2) + 1; // inside boundaries
    rand_y = rand() % (game_y_bound - 2) + 1; // inside boundaries
  } while (rand_x == snake->head->x && rand_y == snake->head->y);

  food->powerup = PU_NONE;

  // rarely, spawn powerup (if allowed)
  if (allow_powerup && (rand() % 100) <= PU_SPAWN_PERCENTAGE)
    food->powerup = rand_powerup();

  food->x = rand_x;
  food->y = rand_y;

  food->consumed = false;
}


/*
 * snake functions
 */

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

  // if not in GS_RUNNING, disallow velocity changes entirely
  if (GS_RUNNING != game_state)
    return;

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


/*
 * gamestate functions
 */

/**
 * function:  gamestate_set
 * ------------------------
 * update the state machine by moving to the specified state.
 *
 * new_gs: the new gamestate to enter
 *
 * returns: true if the state was updated, false if not able to enter this state
 */
bool gamestate_set(enum gamestate_t new_gs)
{
  // TODO when entering the pause state from GS_RUNNING,
  // TODO we should take snake->powerup_expire_ns and subtract from it
  // TODO the value returned by get_time_ns(). This will provide the # of
  // TODO nanoseconds of powerup remaining. When re-entering GS_RUNNING
  // TODO we simply set snake->powerup_expire_ns to get_time_ns() + the value

  bool can_transition = gamestate_can_transition(game_state, new_gs);

  // TODO change game_state to cur_gs

  if (can_transition)
  {
    // handle entering a new gamestate
    switch (new_gs)
    {
      // TODO
    }

    game_state = new_gs;
  }

  return can_transition;
}


static bool gamestate_can_transition(enum gamestate_t from, enum gamestate_t to)
{
  bool can_transition;

  // configure illegal state transitions
  switch (from)
  {
    // from GS_STARTING, we can only enter GS_RUNNING or GS_ENDING
    case GS_STARTING:
      can_transition = (GS_RUNNING == to || GS_ENDING == to);
      break;

    // from GS_RUNNING, we can only enter GS_PAUSED or GS_ENDING
    case GS_RUNNING:
      can_transition = (GS_PAUSED == to || GS_ENDING == to);
      break;

    // from GS_PAUSED, we can only enter GS_RUNNING or GS_ENDING
    case GS_PAUSED:
      can_transition = (GS_RUNNING == to || GS_ENDING == to);
      break;

    // from GS_ENDING, we can only enter GS_STARTING
    // TODO handle restarting game from game over screen
    case GS_ENDING:
      can_transition = (GS_STARTING == to);
      break;

    // by default we can enter this new state
    default:
      can_transition = true;
      break;
  }

  return can_transition;
}


/**
 * function:  gamestate_to_string
 * ------------------------------
 * TODO - Documentation
 */
const char * gamestate_to_string(enum gamestate_t gamestate)
{
  const char * gamestate_names[GS_COUNT];

  gamestate_names[GS_STARTING] = "WELCOME";
  gamestate_names[GS_RUNNING]  = "TTY-SNAKE";
  gamestate_names[GS_PAUSED]   = "PAUSED";
  gamestate_names[GS_ENDING]   = "GAME OVER";


  if (GS_COUNT == gamestate)
    return NULL;
  else
    return gamestate_names[gamestate];
}


/*
 * powerup functions
 */

/**
 * function:  powerup_to_string
 * ----------------------------
 * TODO - Documentation
 */
const char * powerup_to_string(enum powerup_t powerup)
{
  const char * powerup_names[PU_COUNT + 1]; // +1 to include PU_NONE

  powerup_names[PU_NONE + 1]        = "NONE";
  powerup_names[PU_SINGLESTEP + 1]  = "SINGLE-STEP";
  powerup_names[PU_NOGROW + 1]      = "NO GROW";

  if (PU_COUNT == powerup)
    return NULL;
  else
    return powerup_names[powerup + 1];
}

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

/*
 * function:  powerup_activate
 * ---------------------------
 * TODO - Documentation
 */
static void powerup_activate(
    struct game_updatecycle_info * p_uc_info,
    enum   powerup_t               powerup
)
{
  snake->powerup           = powerup;
  snake->powerup_expire_ns = p_uc_info->start_ns + powerup_durations[powerup];

  // don't waste time checking expiry for newly-acquired powerup
  powerup_tick(p_uc_info, false);
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

