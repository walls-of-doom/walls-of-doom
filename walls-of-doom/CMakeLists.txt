cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_MODULE_PATH ../cmake)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
endif ()

configure_file(version.h.in version.h)
configure_file(constants.h.in constants.h)

set(walls-of-doom-sources
        about.h about.c
        bank.h bank.c
        base-io.h base-io.c
        box.h box.c
        clock.h clock.c
        code.h code.c
        color.h color.c
        command.h command.c
        constants.h
        data.h data.c
        game.h game.c
        graphics.h graphics.c
        high-io.h high-io.c
        investment.h investment.c
        joystick.h joystick.c
        logger.h logger.c
        memory.h memory.c
        menu.h menu.c
        numeric.h numeric.c
        perk.h perk.c
        physics.h physics.c
        platform.h platform.c
        player.h player.c
        point.h point.c
        profiler.h profiler.c
        random.h random.c
        record.h record.c
        score.h
        settings.h settings.c
        sort.h sort.c
        text.h text.c
        version.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(walls-of-doom-base ${walls-of-doom-sources})
if (UNIX)
    target_link_libraries(walls-of-doom-base m)
endif (UNIX)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(walls-of-doom-base ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_executable(walls-of-doom main.c ${walls-of-doom-sources})
if (UNIX)
    target_link_libraries(walls-of-doom m)
endif (UNIX)
target_link_libraries(walls-of-doom ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

# Copy the launcher to the binary directory.
configure_file(${CMAKE_SOURCE_DIR}/launcher/start-walls-of-doom.sh ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Copy the assets to the binary directory.
add_custom_command(TARGET walls-of-doom POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/)
