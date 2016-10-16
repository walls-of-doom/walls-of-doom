cmake_minimum_required (VERSION 2.8.7)

project (walls-of-doom)

set (CMAKE_MODULE_PATH cmake)

# Get Git version, if we can.
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND git describe --abbrev=4 HEAD OUTPUT_VARIABLE version-string OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    set (version-string "v1.2.0")
endif ()

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Even though Clang and GCC seem to require specific flags for ISO C90,
# preserving this line seems the right thing to do as it may help with
# unlisted compilers in our conditional chain below.
set (C_STANDARD 90)

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

option (m32 "Generate code for a 32-bit environment.")
option (m64 "Generate code for a 64-bit environment.")

# C standard is enforced with compiler-specific flags.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set (CMAKE_C_FLAGS_BASE "-Wall -Wextra -Werror -Wpedantic")
    if (m32)
        set (CMAKE_C_FLAGS_BASE "${CMAKE_C_FLAGS_BASE} -m32")
    elseif (m64)
        set (CMAKE_C_FLAGS_BASE "${CMAKE_C_FLAGS_BASE} -m64")
    endif ()
endif ()

add_subdirectory (unity)
add_subdirectory (walls-of-doom)
add_subdirectory (tests)
