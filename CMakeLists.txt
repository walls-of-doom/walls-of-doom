cmake_minimum_required (VERSION 2.8.7)

project (walls-of-doom)

set (CMAKE_MODULE_PATH cmake)

# Get Git version, if we can.
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND git describe --abbrev=4 HEAD OUTPUT_VARIABLE version-string OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    set (version-string "v1.1.2")
endif ()

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Even though Clang and GCC seem to require specific flags for ISO C90,
# preserving this line seems the right thing to do as it may help with
# unlisted compilers in our conditional chain below.
set (C_STANDARD 90)

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

# C standard is enforced with compiler-specific flags.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set (CMAKE_C_FLAGS "-std=c89 -Wall -Wextra -Werror -Wpedantic")
endif ()

add_subdirectory (unity)
add_subdirectory (walls-of-doom)
add_subdirectory (tests)
