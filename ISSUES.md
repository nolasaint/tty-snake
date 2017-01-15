### KNOWN BUGS

* if compiled to use the keyboard listener thread, a few issues arise:
  * snake movement becomes buggy
  * have to handle signals/interrupts inside pthread so that exiting does not require extra input

### TO-DO

* implement rates for game_update, graphics_update
  * use the same strategy for these as we do for the engine tickrate limiter

* rarely, randomly choose a powerup for newly-spawning food

* display food with powerups differently

* add collision map (2D bool array with an entry for each coordinate)

* add collision detection
