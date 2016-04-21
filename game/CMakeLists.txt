cmake_minimum_required (VERSION 2.9)

set (CMAKE_BUILD_TYPE Release)

add_executable (walls-of-doom walls-of-doom.c)

target_link_libraries (walls-of-doom walls-of-doom-source)
