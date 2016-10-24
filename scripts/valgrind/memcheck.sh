#!/usr/bin/env bash
#
# This is a single-command script to run Memcheck.
#
cd build/walls-of-doom/
valgrind --leak-check=full --track-origins=yes ./walls-of-doom
cd ../../
