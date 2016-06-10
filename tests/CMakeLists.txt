cmake_minimum_required (VERSION 2.8.7)

add_executable (tests tests.c)

target_link_libraries (tests walls-of-doom-source)
target_link_libraries (tests unity)
