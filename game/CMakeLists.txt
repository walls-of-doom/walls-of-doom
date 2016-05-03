cmake_minimum_required (VERSION 2.9)

set (CMAKE_BUILD_TYPE Release)

add_executable (walls-of-doom walls-of-doom.c)

target_link_libraries (walls-of-doom walls-of-doom-source)

set_target_properties (walls-of-doom PROPERTIES LINKER_LANGUAGE C C_STANDARD 90)

find_package( Curses REQUIRED )
include_directories( ${CURSES_INCLUDE_DIRS} )
target_link_libraries( walls-of-doom ${CURSES_LIBRARIES} )
