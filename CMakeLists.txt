cmake_minimum_required (VERSION 2.8.7)

project (walls-of-doom)

set (CMAKE_MODULE_PATH cmake)

# Get Git version, if we can.
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND git describe --abbrev=4 HEAD OUTPUT_VARIABLE version-string OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    set (version-string "v1.3.1")
endif ()

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

option (ENV32 "Generate code for a 32-bit environment.")
option (ENV64 "Generate code for a 64-bit environment.")
option (SANITIZE "Modify the program at compile-time to catch undefined behavior during program execution.")
option (OPTIMIZE_SIZE "Optimize for program size.")

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")
    if (ENV32)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    elseif (ENV64)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    endif ()
    if (OPTIMIZE_SIZE)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
    endif ()
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    if (SANITIZE)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    endif ()
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_NO_WARNINGS=1")
endif ()

add_subdirectory (walls-of-doom)

if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    add_subdirectory (unity)
    add_subdirectory (tests)
endif ()
