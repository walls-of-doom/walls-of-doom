#!/usr/bin/env bash
#
# This is a single-command script to run Memcheck.
valgrind --leak-check=full --track-origins=yes build/walls-of-doom/walls-of-doom
