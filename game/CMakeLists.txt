cmake_minimum_required (VERSION 2.8.7)

configure_file (version.h.in version.h)

set (CMAKE_MODULE_PATH ../cmake)

add_library (walls-of-doom-source
    about.h about.c
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

set_target_properties (walls-of-doom-source PROPERTIES LINKER_LANGUAGE C)

find_package (Curses REQUIRED)
include_directories (${CURSES_INCLUDE_DIRS})
target_link_libraries (walls-of-doom-source ${CURSES_LIBRARIES})

find_package (SDL2 REQUIRED)
find_package (SDL2TTF REQUIRED)
include_directories (${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})
target_link_libraries (walls-of-doom-source ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY})

target_include_directories (walls-of-doom-source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (game game.c)
target_link_libraries (game walls-of-doom-source)
set_target_properties (game PROPERTIES LINKER_LANGUAGE C)

# Copy the assets to the binary directory.
add_custom_command (TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/)
