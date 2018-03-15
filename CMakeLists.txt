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

set(CMAKE_CXX_STANDARD 14)
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

configure_file(sources/version.hpp.in version.hpp)
configure_file(sources/constants.hpp.in constants.hpp)

set(walls-of-doom-sources
        sources/about.hpp
        sources/about.cpp
        sources/bank.hpp
        sources/bank.cpp
        sources/base-io.hpp
        sources/base-io.cpp
        sources/box.hpp
        sources/box.cpp
        sources/clock.hpp
        sources/clock.cpp
        sources/code.hpp
        sources/code.cpp
        sources/color.hpp
        sources/color.cpp
        sources/command.hpp
        sources/command.cpp
        sources/constants.hpp
        sources/data.hpp
        sources/data.cpp
        sources/integers.hpp
        sources/game.hpp
        sources/game.cpp
        sources/graphics.hpp
        sources/graphics.cpp
        sources/high-io.hpp
        sources/high-io.cpp
        sources/investment.hpp
        sources/investment.cpp
        sources/joystick.hpp
        sources/joystick.cpp
        sources/logger.hpp
        sources/logger.cpp
        sources/memory.hpp
        sources/memory.cpp
        sources/menu.hpp
        sources/menu.cpp
        sources/numeric.hpp
        sources/numeric.cpp
        sources/perk.hpp
        sources/perk.cpp
        sources/physics.hpp
        sources/physics.cpp
        sources/platform.hpp
        sources/platform.cpp
        sources/player.hpp
        sources/player.cpp
        sources/point.hpp
        sources/point.cpp
        sources/profiler.hpp
        sources/profiler.cpp
        sources/random.hpp
        sources/random.cpp
        sources/record.hpp
        sources/record.cpp
        sources/score.hpp
        sources/settings.hpp
        sources/settings.cpp
        sources/sort.hpp
        sources/sort.cpp
        sources/text.hpp
        sources/text.cpp
        sources/version.hpp
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
