# tty-snake

unix terminal-based rendition of the classic 'snake' arcade game.


## Table of Contents

1. [About](#about)
2. [Compilation](#compilation)
3. [Usage](#usage)
4. [Gameplay](#gameplay)
5. [Program Description](#program-description)


## About

lorem ipsum


## Compilation

These are the `make` targets for this project:

| Target | Description |
|-------:|-------------|
| _(none)_ | default compilation |
| all | default compilation |
| clean | removes all compiled files |
| debug | compiles with `-g3` flag to disable optimization |

For standard compilation, use:

```bash
$ make
```

To delete all compiled / output files, use:

```
$ make clean
```

## Usage

Once the program is compiled, run it with:

```bash
$ ./tty-snake
```

The program can be stopped at any time by pressing `Ctrl-C`.


## Gameplay

Control the direction that an on-screen snake moves in while collecting food and avoding collisions with the walls and itself.

Controls:

| Button | Description |
|:------:|-------------|
| W | change the velocity to upward (also: up arrow) |
| A | change the velocity to leftward (also: left arrow) |
| S | change the velocity to downward (also: down arrow) |
| D | change the velocity to rightward (also: right arrow) |
| Q | quit the game |


**TODO** describe powerups


**TODO** describe scoring


## Program Description

*Note*: _see the [wiki](program-description) for a more detailed description_.

[ ROUGH DRAFT]

Game is broken down into 3 major segments: the program engine, game engine, and graphics engine.

The program engine, found in `engine.c` contains the bulk of the code that coordinates the different parts of the game. It controls the rate at which the game and graphics engines run, listens for and handles keyboard input, and redirects program flow to respond to different game states.

