## KNOWN BUGS

#### Keyboard Input

* if compiled to use the keyboard listener thread, a few issues arise:
  * snake movement becomes buggy
  * have to handle signals/interrupts inside pthread so that exiting does not require extra input

#### Powerups

* powerups spawn far too frequently, check logic in food_spawn

## TO-DO

#### Engine

* [POSTPONED] implement rates for game_update, graphics_update
  * use the same strategy for these as we do for the engine tickrate limiter

#### Powerups

* randomly choose powerups based on powerup probability

* define how food with a powerup should be displayed (for each powerup)

* enforce a lifetime for powerups so they do not last forever, and display this lifetime on the screen

#### Collision Detection

* [POSTPONED] add collision map (2D bool array with an entry for each coordinate)

* when single-step powerup is active, disallow backtracking by checking the previous velocity
