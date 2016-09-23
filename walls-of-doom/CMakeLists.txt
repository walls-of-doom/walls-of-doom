cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_MODULE_PATH ../cmake)

configure_file (version.h.in version.h)

add_library (walls-of-doom-base
    about.h about.c
    codes.h
    command.h
    constants.h
    data.h data.c
    game.h game.c
    io.h io.c
    logger.h logger.c
    math.h math.c
    menu.h menu.c
    perks.h perks.c
    physics.h physics.c
    platform.h platform.c
    player.h player.c
    random.h random.c
    record.h record.c
    rest.h rest.c
    safe.h safe.c
    sort.h sort.c
    types.h
    version.h)

find_package (SDL2 REQUIRED)
find_package (SDL2TTF REQUIRED)
find_package (SDL2_IMAGE REQUIRED)
include_directories (${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries (walls-of-doom-base ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY})

target_include_directories (walls-of-doom-base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (walls-of-doom game.c)
target_link_libraries (walls-of-doom walls-of-doom-base)

add_executable (tests tests.c)
target_link_libraries (tests unity)
target_link_libraries (tests walls-of-doom-base)

# Copy the assets to the binary directory.
add_custom_command (TARGET walls-of-doom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/)
