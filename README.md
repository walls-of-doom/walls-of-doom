# Walls of Doom

Walls of Doom is a minimalistic
[platformer](https://en.wikipedia.org/wiki/Platform_game) written in [ANSI
C](https://en.wikipedia.org/wiki/ANSI_C). The game is still under development
but is already playable.

See [the GitHub issue
tracker](https://github.com/walls-of-doom/walls-of-doom/issues) and have a look
at [the releases page](https://github.com/walls-of-doom/walls-of-doom/releases).

# Continuous Integration Status

## Semaphore CI

This build server uses Ubuntu 14.04 64-bit, CMake 2.8, and GCC 4.8.4.

The [build status](https://semaphoreci.com/walls-of-doom/walls-of-doom) is
visible to everyone.

# Building and Running

This project uses SDL 2.0 and you must have it in order to build the game.

The following C compilers are known to work perfectly with the project:

+ GCC 4.8.4 (or newer)
+ Clang 3.7 (or newer)

## Building and installing

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

## Running the game

```bash
$ walls-of-doom
```

## Running the tests

```bash
$ ./build/walls-of-doom/tests
```

## Reformatting code and performing static analysis

```bash
$ bash reformat.sh
```

# Generating the Images

```bash
$ cd blender
$ bash render.sh
```

And they should be written in to the output subdirectory.

## Links

All these images are posted on the [Imgur album of the project](http://imgur.com/a/kiOY2).

# Implementation Notes

## IO

Before using the IO capabilities of Walls of Doom, `initialize()` must be
called. Before quitting the game, `finalize()` must be called to free associated
resources.

If one must use the logger without initializing the other IO functions, the
logger module may be independently initialized by calling `initialize_logger()`
and finalized by calling `finalize_logger()`.

## Business Logic

### Menu

Under the Menu name is grouped most of the logic that deals with handling user
interactions outside of the game. This includes menu item selection, the top
scores functionality and the auxiliary functions of these parts of the
application.

### Physics

The Physics module is where most of the calculations and checks take place.
Walking, falling, jumping, the fetching of perks, and death are some of the
things that fall under the scope of this module.

## Sorting

This project has its own generic insertion sort implementation.

It is a generic function because it uses void pointers and function pointers to
comparators in order to be reused for different data types without any code
duplication.

## Pseudorandom Number Generator

Before using the PRNG, you can initialize it with the current time by calling
`seed_random()`.

This project uses the **[xoroshiro+](http://xoroshiro.di.unimi.it/)** algorithm
to efficiently generate pseudo-random numbers with a big period.

The convenience function that returns an integer in the specified range uses
multiple random numbers modulo the next power of two to prevent the modulo bias
that comes with more naive approaches.

# License

It is licensed under the BSD 3-Clause license. See LICENSE.txt for more
information.
