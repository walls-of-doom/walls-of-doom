cmake_minimum_required (VERSION 2.8.7)

add_executable (tests tests.c)

include (FindPkgConfig)

pkg_search_module (SDL2 REQUIRED sdl2)
include_directories (${SDL2_INCLUDE_DIRS})
target_link_libraries (tests ${SDL2_LIBRARIES})

target_link_libraries (tests walls-of-doom-source)
target_link_libraries (tests unity)
