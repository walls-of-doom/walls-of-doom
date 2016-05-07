cmake_minimum_required (VERSION 2.8.7)

configure_file (version.h.in version.h)

add_library (walls-of-doom-source array.h data.h io.h log.h platform.h record.h rest.h sort.h version.h)

set_target_properties (walls-of-doom-source PROPERTIES LINKER_LANGUAGE C)

target_include_directories (walls-of-doom-source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
