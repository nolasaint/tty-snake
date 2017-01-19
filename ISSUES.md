## KNOWN BUGS

#### Keyboard Input

* if compiled to use the keyboard listener thread, a few issues arise:
  * snake movement becomes buggy
  * have to handle signals/interrupts inside pthread so that exiting does not require extra input

* if the kyeboard input thread is NOT used, input will be buffered and can be read late
  * can be seen clearly when in single-step mode and holding down a directional arrow, then letting go

#### Movement

* in single-step mode, snake can backtrack (since velocity is VEL_NONE)
  * can be fixed by tracking previous velocity in addition to current velocity

* in single-step mode, when the powerup expires, the snake can remain at VEL_NONE
  * again, can be fixed by tracking previous velocity

## TO-DO

#### Scoring

* implement scoring (factor of length, time, and food eaten, maybe?)

* display score

#### Engine

* [POSTPONED] implement rates for game_update, graphics_update
  * use the same strategy for these as we do for the engine tickrate limiter

#### Powerups

* randomly choose powerups based on powerup probability

* define how food with a powerup should be displayed (for each powerup)

* display how until a powerup will expire

#### Collision Detection

* [POSTPONED] add collision map (2D bool array with an entry for each coordinate)

* when single-step powerup is active, disallow backtracking by checking the previous velocity
