cmake_minimum_required (VERSION 2.8.7)

configure_file (version.h.in version.h)

add_library (walls-of-doom-source
    about.h about.c
    command.h
    constants.h
    data.h data.c
    io.h io.c
    logger.h logger.c
    math.h math.c
    physics.h physics.c
    platform.h
    player.h player.c
    random.h random.c
    record.h record.c
    rest.h rest.c
    sort.h sort.c
    vector.h vector.c
    version.h)

set_target_properties (walls-of-doom-source PROPERTIES LINKER_LANGUAGE C)

target_include_directories (walls-of-doom-source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
