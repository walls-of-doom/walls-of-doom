#!/usr/bin/env bash

# This is the script used to run the tests in Semaphore CI.
cd build
make
./tests
cd ..
