cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_MODULE_PATH ../cmake)

add_executable (tests tests.c)

find_package (SDL2 REQUIRED)
find_package (SDL2TTF REQUIRED)
include_directories (${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})
target_link_libraries (tests ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY})

target_link_libraries (tests walls-of-doom-source)
target_link_libraries (tests unity)
