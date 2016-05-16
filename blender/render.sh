#!/usr/bin/bash
#
# This bash script renders all the images using Blender and Python

blender --background name.blend --python instructions.py
blender --background name-with-lamp.blend --python instructions.py
blender --background name-over-bricks.blend --python instructions.py
