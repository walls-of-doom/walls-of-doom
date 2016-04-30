Walls of Doom
=============

About
-----

Walls of Doom (WoD) is a minimalistic
[platformer](https://en.wikipedia.org/wiki/Platform_game) written in C by
Bernardo Sulzbach and Leonardo Ferrazza.

Building and Running
--------------------

### Terminal

```bash
$ cmake .
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

Implementation Notes
--------------------

**insertion-sort.h**

+ Our generic insertion sort implementation

We only need to keep track of the five best scores. Therefore, insertion sort is
a good enough algorithm for our requirements.

License
-------

It is licensed under the BSD 3-Clause license. See LICENSE.txt for more
information.
