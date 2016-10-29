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

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

option (ENV32 "Generate code for a 32-bit environment.")
option (ENV64 "Generate code for a 64-bit environment.")
option (SANITIZE "Modify the program at compile-time to catch undefined behavior during program execution.")

set (COMPILER_IS_GNU ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
set (COMPILER_IS_CLANG ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"))
set (COMPILER_IS_GNU_OR_CLANG ((COMPILER_IS_GNU) OR (COMPILER_IS_CLANG)))

if (COMPILER_IS_GNU_OR_CLANG)
    set (CMAKE_C_FLAGS_BASE "-Wall -Wextra -Werror -Wpedantic")
    if (ENV32)
        set (CMAKE_C_FLAGS_BASE "${CMAKE_C_FLAGS_BASE} -m32")
    elseif (ENV64)
        set (CMAKE_C_FLAGS_BASE "${CMAKE_C_FLAGS_BASE} -m64")
    endif ()
endif ()

if (COMPILER_IS_CLANG)
    if (SANITIZE)
        set (CMAKE_C_FLAGS_BASE "${CMAKE_C_FLAGS_BASE} -fsanitize=undefined")
    endif ()
endif ()

add_subdirectory (unity)
add_subdirectory (walls-of-doom)
add_subdirectory (tests)
