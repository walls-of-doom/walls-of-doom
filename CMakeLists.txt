cmake_minimum_required(VERSION 2.8.7)

project(walls-of-doom LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Get Git version, if we can.
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND git describe --abbrev=4 HEAD OUTPUT_VARIABLE version-string OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    set(version-string "v1.4.0")
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

option(ENV32 "Generate code for a 32-bit environment.")
option(ENV64 "Generate code for a 64-bit environment.")
option(SANITIZE "Modify the program at compile-time to catch undefined behavior during program execution.")
option(OPTIMIZE_SIZE "Optimize for program size.")

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-Wall)
    add_definitions(-Wextra)
    add_definitions(-Werror)
    if (ENV32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    elseif (ENV64)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    endif ()
    if (OPTIMIZE_SIZE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
    endif ()
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    if (SANITIZE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    endif ()
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_NO_WARNINGS=1")
endif ()

configure_file(sources/version.h.in version.h)
configure_file(sources/constants.h.in constants.h)

set(walls-of-doom-sources
        sources/about.h
        sources/about.cpp
        sources/bank.h
        sources/bank.cpp
        sources/base-io.h
        sources/base-io.cpp
        sources/box.h
        sources/box.cpp
        sources/clock.h
        sources/clock.cpp
        sources/code.h
        sources/code.cpp
        sources/color.h
        sources/color.cpp
        sources/command.h
        sources/command.cpp
        sources/constants.h
        sources/data.h
        sources/data.cpp
        sources/integers.h
        sources/game.h
        sources/game.cpp
        sources/graphics.h
        sources/graphics.cpp
        sources/high-io.h
        sources/high-io.cpp
        sources/investment.h
        sources/investment.cpp
        sources/joystick.h
        sources/joystick.cpp
        sources/logger.h
        sources/logger.cpp
        sources/memory.h
        sources/memory.cpp
        sources/menu.h
        sources/menu.cpp
        sources/numeric.h
        sources/numeric.cpp
        sources/perk.h
        sources/perk.cpp
        sources/physics.h
        sources/physics.cpp
        sources/platform.h
        sources/platform.cpp
        sources/player.h
        sources/player.cpp
        sources/point.h
        sources/point.cpp
        sources/profiler.h
        sources/profiler.cpp
        sources/random.h
        sources/random.cpp
        sources/record.h
        sources/record.cpp
        sources/score.h
        sources/settings.h
        sources/settings.cpp
        sources/sort.h
        sources/sort.cpp
        sources/text.h
        sources/text.cpp
        sources/version.h
        )

add_library(walls-of-doom-base ${walls-of-doom-sources})

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}}/sources")
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

target_link_libraries(walls-of-doom-base ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_executable(walls-of-doom sources/main.cpp ${walls-of-doom-sources})

target_link_libraries(walls-of-doom ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_custom_command(TARGET walls-of-doom POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/)

if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    add_executable(tests tests/tests.cpp catch/catch.hpp)
    include_directories("${CMAKE_SOURCE_DIR}}/sources")
    target_link_libraries(tests walls-of-doom-base)
    target_link_libraries(tests ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
endif ()
