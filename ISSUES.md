## KNOWN BUGS

#### Gameplay

* unable to capture Enter key (notice this on game over screen)

* if game is paused, powerup timer still decreases

#### Graphics

* popup windows can sometimes cause game elements behind it to be erased

#### Keyboard Input

* if compiled to use the keyboard listener thread, a few issues arise:
  * snake movement becomes buggy
  * have to handle signals/interrupts inside pthread so that exiting does not require extra input

* if the keyboard input thread is NOT used, input will be buffered and can be read late
  * can be seen clearly when in single-step mode and holding down a directional arrow, then letting go

## TO-DO

#### Gameplay

* allow game to be restarted from GS_ENDING

#### Scoring

* change how scoring works (currently just increments when food is consumed, and takes snake length into account)

#### Engine

* [POSTPONED] implement rates for game_update, graphics_update
  * use the same strategy for these as we do for the engine tickrate limiter

#### Powerups

* randomly choose powerups based on powerup probability

* define how food with a powerup should be displayed (for each powerup)

* display how until a powerup will expire

#### Collision Detection

* [POSTPONED] add collision map (2D bool array with an entry for each coordinate)

