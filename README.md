Walls of Doom
=============

# About

Walls of Doom (WoD) is a minimalistic
[platformer](https://en.wikipedia.org/wiki/Platform_game) written in C by
Bernardo Sulzbach and Leonardo Ferrazza.

See [the Bitbucket issue tracker](https://bitbucket.org/mafagafogigante/walls-of-doom/issues?status=new&status=open&sort=-priority).

# Continuous Integration Status

## Semaphore CI

This build server uses Ubuntu 14.04 64-bit, CMake 2.8, and GCC 4.8.4.

Everyone can see the [build status](https://semaphoreci.com/mafagafogigante/walls-of-doom).

# Building and Running

The following C compilers are known to work perfectly with the project:

+ GCC 4.8
+ GCC 5.3
+ Clang 3.7

## Terminal

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Running the tests

```bash
$ cd tests
$ ./tests
```

### Running the game

```bash
$ cd game
$ ./game
```

## Code::Blocks

```bash
$ cmake . -G"CodeBlocks - Unix Makefiles"
```

### Running the tests

+ Open Code::Blocks
+ Open the Code::Blocks project file on the root directory
+ Select `autotest` as the target
+ Press "Build and run"

### Running the game

+ Open Code::Blocks
+ Open the Code::Blocks project file on the root directory
+ Select `walls-of-doom` as the target
+ Press "Build and run"

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
