cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_BUILD_TYPE Release)

add_executable (autotest autotest.c)

target_link_libraries (autotest walls-of-doom-source)
target_link_libraries (autotest unity)
