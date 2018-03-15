#!/usr/bin/env bash

# This is the script used to setup the environment for Semaphore CI.
sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
mkdir build
cd build
cmake ..
make

# Go back to the project root.
cd ..
