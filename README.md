# Walls of Doom

Walls of Doom is a minimalistic [platformer](https://en.wikipedia.org/wiki/Platform_game) written in C++ using SDL 2.

The game was originally written in [ANSI C](https://en.wikipedia.org/wiki/ANSI_C), but I decided to rewrite it using C++, a safer and more productive language. However, this is an ongoing effort and several parts of the codebase are still just C code.

The game should work under Windows, Linux, and everything else that SDL 2 supports.

See [the GitHub issue tracker](https://github.com/walls-of-doom/walls-of-doom/issues) and have a look at [the releases page](https://github.com/walls-of-doom/walls-of-doom/releases).

The physics simulation is resolution-bound, therefore on a very high resolution you will get more pixels per tile and - consequently - a more fine-grained simulation.

## Screenshot

![Screenshot](https://raw.githubusercontent.com/walls-of-doom/walls-of-doom/master/screenshots/screenshot.png)

# Continuous Integration Status

## Travis CI

[![Build Status](https://travis-ci.org/walls-of-doom/walls-of-doom.svg?branch=master)](https://travis-ci.org/walls-of-doom/walls-of-doom)

This build servers uses Ubuntu 14.04.3 64-bit, and builds with the following configurations:

```
Clang         3.5.0
SDL2          2.0.2
SDL2_ttf     2.0.12
SDL2_image    2.0.0
```

```
GCC           4.8.4
SDL2          2.0.2
SDL2_ttf     2.0.12
SDL2_image    2.0.0
```

## Semaphore CI

[![Build Status](https://semaphoreci.com/api/v1/walls-of-doom/walls-of-doom/branches/master/shields_badge.svg)](https://semaphoreci.com/walls-of-doom/walls-of-doom)

This build server uses Ubuntu 14.04 64-bit and builds with the following configuration:

```
GCC           4.8.4
SDL2          2.0.2
SDL2_ttf     2.0.12
SDL2_image    2.0.0
```

## AppVeyor

[![Build Status](https://ci.appveyor.com/api/projects/status/1gparjoqw7upxhod/branch/master?svg=true)](https://ci.appveyor.com/project/walls-of-doom/walls-of-doom/branch/master)

This build servers builds under Windows using Visual Studio 2015 for Visual Studio 2010, which is required for Windows XP compatibility.

```
SDL2          2.0.4
SDL2_ttf     2.0.14
SDL2_image    2.0.1
```

# Building and Running

## Requirements

### Hardware

Because software rendering is supported, a graphics card is not required.

The game needs less than 10 MiB of free memory to run.

### Operating System

#### Windows

Windows is fully supported and self-contained binaries are provided.

+ Windows XP or later is required.
+ The C++ redistributables provided by Microsoft are required.

#### Linux and BSD

The game works on any Linux or BSD OS with a desktop environment.

#### macOS

OS X and macOS support is unknown. [See the relevant issue](https://github.com/walls-of-doom/walls-of-doom/issues/39).

### CMake

You need an installation of CMake in order to build the project.

## SDL 2

You will need SDL 2, with the TrueType and Image libraries.

## Building

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

> You can pass `-DENV32=1` to CMake to get a 32-bit build on a 64-bit system.
>
> Note that to build the 32-bit version, you need 32-bit versions of all the dependencies.
>
> You can pass `-DSANITIZE=1` to CMake to get the LLVM undefined behavior sanitizer.

It is suggested that you pass `-DCMAKE_BUILD_TYPE="Release"` to CMake if you want a build to play the game, as it will generate more optimized code.

## Running the game

```bash
$ bash build/walls-of-doom/start-walls-of-doom.sh
```

## Running the tests

```bash
$ bash scripts/test.sh
```

# Coding Style

The coding style (LLVM style, but for C) is enforced by LLVM tools.

## Running the formatter

```bash
$ bash scripts/llvm/reformat.sh
```

## Performing static analysis

```bash
$ bash scripts/llvm/analyze.sh
```

## Include Order

1. Main header
2. Walls of Doom headers
3. SDL 2 headers
4. System headers

This include order should be respected in all source files.

# License

It is licensed under the BSD 3-Clause license. See LICENSE.txt for more information.
