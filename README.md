# Walls of Doom

Walls of Doom is a minimalistic [platformer](https://en.wikipedia.org/wiki/Platform_game) written in [ANSI C](https://en.wikipedia.org/wiki/ANSI_C) using SDL 2.

The game is still under development but is already playable on Linux.

See [the GitHub issue tracker](https://github.com/walls-of-doom/walls-of-doom/issues)
and have a look at [the releases page](https://github.com/walls-of-doom/walls-of-doom/releases).

# Continuous Integration Status

## Travis CI

[![Build Status](https://travis-ci.org/walls-of-doom/walls-of-doom.svg?branch=master)](https://travis-ci.org/walls-of-doom/walls-of-doom)

This build servers uses Ubuntu 14.04.3 64-bit, and builds with the following
configurations:

```
Clang 3.5.0
SDL2 2.0.2
SDL2_ttf 2.0.12
SDL2_image 2.0.0
```

```
GCC 4.8.4
SDL2 2.0.2
SDL2_ttf 2.0.12
SDL2_image 2.0.0
```

## Semaphore CI

[![Build Status](https://semaphoreci.com/api/v1/walls-of-doom/walls-of-doom/branches/master/shields_badge.svg)](https://semaphoreci.com/walls-of-doom/walls-of-doom)

This build server uses Ubuntu 14.04 64-bit and builds with the following
configuration:

```
GCC 4.8.4
SDL2 2.0.2
SDL2_ttf 2.0.12
SDL2_image 2.0.0
```

The [build status](https://semaphoreci.com/walls-of-doom/walls-of-doom) is
visible to everyone.

# Building and Running

## Requirements

### Linux

The game should work on any Linux or BSD operating system with a desktop
environment.

Currently, there is no Windows support. [See the relevant
issue](https://github.com/walls-of-doom/walls-of-doom/issues/38).

OS X and macOS support is unknown. [See the relevant
issue](https://github.com/walls-of-doom/walls-of-doom/issues/39).

### CMake

You need an installation of CMake in order to build and install the project.
The suggested 2.8.7 version in the CMake files distributed with the project may
not be enough for what we use.

### An ANSI C compiler

The following C compilers are known to work perfectly with the project:

+ GCC 4.8.4 (or newer)
+ Clang 3.7 (or newer)

Older versions of the abovementioned compilers are **very** likely to work too.

## SDL 2

You will need SDL 2, with the TrueType and Image libraries.

## Building and installing

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

> You can use `cmake -Dm32=1 ..` to get a 32-bit build on a 64-bit system.

> Note that in order to build this 32-bit version, you will need the 32-bit
> versions of all project dependencies.

## Running the game

```bash
$ walls-of-doom
```

## Running the tests

```bash
$ ./build/tests/tests
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

These are notes on some of the modules of the project to help new developers get
to know the project structure better without having to look into the source
code.

## IO

Before using the IO capabilities of Walls of Doom, `initialize()` must be
called. Before quitting the game, `finalize()` must be called to free associated
resources.

## Logger

Walls of Doom has a very simple logger which can be used by the `log_message()`
function. If for some reason logging fails, it fail silently and the program is
not interrupted.

If one must use the logger without initializing the other IO functions, the
logger module may be independently initialized by calling `initialize_logger()`
and finalized by calling `finalize_logger()`.

## Menu

Under the Menu name is grouped most of the logic that deals with handling user
interactions outside of the game. This includes menu item selection, the top
scores functionality and the auxiliary functions of these parts of the
application.

## Physics

The Physics module is where most of the calculations and checks take place.
Walking, falling, jumping, the fetching of perks, and death are some of the
things that fall under the scope of this module.

## Random

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
