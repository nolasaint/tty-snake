### KNOWN BUGS

* if compiled to use the keyboard listener thread, a few issues arise:
  * snake movement becomes buggy
  * have to handle signals/interrupts inside pthread so that exiting does not require extra input

### TO-DO

* (EVENTUALLY) implement rates for game_update, graphics_update
  * use the same strategy for these as we do for the engine tickrate limiter

* rarely, randomly choose a powerup for newly-spawning food

* display food with powerups differently

* (PROBABLY NOT HAPPENING) add collision map (2D bool array with an entry for each coordinate)

* when single-step powerup is active, disallow backtracking by checking the previous velocity
