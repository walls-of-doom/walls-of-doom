Walls of Doom
=============

# About

Walls of Doom (WoD) is a minimalistic
[platformer](https://en.wikipedia.org/wiki/Platform_game) written in C by
Bernardo Sulzbach and Leonardo Ferrazza.

See [the Bitbucket issue tracker](https://bitbucket.org/mafagafogigante/walls-of-doom/issues?status=new&status=open&sort=-priority).

# Building and Running

### Terminal

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

#### Running the tests

```bash
$ ./autotest/autotest
```

#### Running the game

```bash
$ ./game/walls-of-doom
```

### Code::Blocks

```bash
$ cmake . -G"CodeBlocks - Unix Makefiles"
```

#### Running the tests

+ Open Code::Blocks
+ Open the Code::Blocks project file on the root directory
+ Select `autotest` as the target
+ Press "Build and run"

#### Running the game

+ Open Code::Blocks
+ Open the Code::Blocks project file on the root directory
+ Select `walls-of-doom` as the target
+ Press "Build and run"

# Implementation Notes

## **insertion-sort.h**

+ Our generic insertion sort implementation

We only need to keep track of the five best scores. Therefore, insertion sort is
a good enough algorithm for our requirements.

## **random.h**

We are using the **[xoroshiro+](http://xoroshiro.di.unimi.it/)** algorithm to
efficiently generate pseudo-random numbers with a big period.

The convenience function that returns an integer in the specified range uses
multiple random numbers modulo the next power of two to prevent the modulo bias
that comes with more naive approaches.

License
-------

It is licensed under the BSD 3-Clause license. See LICENSE.txt for more
information.
