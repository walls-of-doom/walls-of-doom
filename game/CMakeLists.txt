cmake_minimum_required (VERSION 2.8.7)

add_executable (walls-of-doom walls-of-doom.c)

target_link_libraries (walls-of-doom walls-of-doom-source)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
target_link_libraries(walls-of-doom ${CURSES_LIBRARIES})
