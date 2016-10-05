cmake_minimum_required (VERSION 2.8.7)

project (walls-of-doom)

set (CMAKE_MODULE_PATH cmake)

set (version-major 0)
set (version-minor 8)
set (version-patch 0)

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Even though Clang and GCC seem to require specific flags for ISO C90,
# preserving this line seems the right thing to do as it may help with
# unlisted compilers in our conditional chain below.
set (C_STANDARD 90)

set (CMAKE_BUILD_TYPE Release)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

# C standard is enforced with compiler-specific flags.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_C_FLAGS "-std=c89 -Werror -Wpedantic -Wall -Wextra")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set (CMAKE_C_FLAGS "-std=c89 -Werror -Wpedantic -Wall -Wextra")
endif ()

add_subdirectory (unity)
add_subdirectory (walls-of-doom)
add_subdirectory (tests)
