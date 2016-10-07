cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_MODULE_PATH ../cmake)
set (ASSETS_DIR ${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME})
set (WRITABLE_ASSETS_DIR ${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}/saved)

configure_file (version.h.in version.h)
configure_file (constants.h.in constants.h)

add_library (walls-of-doom-base
    about.h about.c
    box.h box.c
    clock.h clock.c
    code.h
    color.h color.c
    command.h
    constants.h
    data.h data.c
    game.h game.c
    io.h io.c
    logger.h logger.c
    memory.h memory.c
    menu.h menu.c
    numeric.h numeric.c
    perk.h perk.c
    physics.h physics.c
    platform.h platform.c
    player.h player.c
    profiler.h profiler.c
    random.h random.c
    record.h record.c
    settings.h settings.c
    sort.h sort.c
    text.h text.c
    version.h)

find_package (SDL2 REQUIRED)
find_package (SDL2TTF REQUIRED)
find_package (SDL2_IMAGE REQUIRED)
include_directories (${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries (walls-of-doom-base ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY})

target_include_directories (walls-of-doom-base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (walls-of-doom main.c)
target_link_libraries (walls-of-doom walls-of-doom-base)

install (TARGETS walls-of-doom RUNTIME DESTINATION bin)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION ${ASSETS_DIR})
