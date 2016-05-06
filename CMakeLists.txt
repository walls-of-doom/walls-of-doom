cmake_minimum_required (VERSION 2.8.7)

project (walls-of-doom)

# The standard is C 90, always.
set (C_STANDARD 90)

set (CMAKE_BUILD_TYPE Release)

# If we got GCC, use -Wall and -Wextra.
if (CMAKE_COMPILER_IS_GNUCC)
    set (CMAKE_C_FLAGS "-Wall -Wextra")
endif ()

add_subdirectory (source)
add_subdirectory (unity)
add_subdirectory (autotest)
add_subdirectory (game)
