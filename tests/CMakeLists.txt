cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_MODULE_PATH ../cmake)

find_package (SDL2 REQUIRED)
find_package (SDL2TTF REQUIRED)
find_package (SDL2_IMAGE REQUIRED)
include_directories (${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})

add_executable (tests tests.c)
target_link_libraries (tests unity)
target_link_libraries (tests walls-of-doom-base)
target_link_libraries (tests ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY})
