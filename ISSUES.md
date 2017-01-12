### KNOWN BUGS

* snake slowly speeds up over time (not currently supposed to)

* * FIX: `clock_settime(CLOCK_PROCESS_CPUTIME_ID, ...)` is returning `-EPERM`, so we will instead calculate the difference between the clock observed at the start of the loop and the clock observed at the end of the loop to determine the length of time to sleep for.

### TO-DO

* limit engine tickrate

* implement rates for game_update, graphics_update

* rarely, randomly choose a powerup for newly-spawning food

* add collision map (2D bool array with an entry for each coordinate)

* add collision detection
