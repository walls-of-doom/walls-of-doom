cmake_minimum_required (VERSION 2.8.7)

add_executable (walls-of-doom walls-of-doom.c)

target_link_libraries (walls-of-doom walls-of-doom-source)

add_custom_command (TARGET walls-of-doom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/ ${CMAKE_BINARY_DIR}/assets/)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
target_link_libraries(walls-of-doom ${CURSES_LIBRARIES})
