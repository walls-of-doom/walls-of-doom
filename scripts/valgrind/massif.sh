#!/usr/bin/env bash
#
# This is a single-command script to run Massif, a heap profiler.
#
cd build/walls-of-doom/
valgrind --tool=massif --stacks=yes --massif-out-file=massif.out ./walls-of-doom
# Could use ms_print instead, but massif-visualizer is more useful.
massif-visualizer massif.out
rm massif.out
cd ../../
