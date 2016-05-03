cmake_minimum_required (VERSION 2.9)

add_library (walls-of-doom-source array.h data.h record.h rest.h sort.h)

set_target_properties (walls-of-doom-source PROPERTIES LINKER_LANGUAGE C C_STANDARD 90)

target_include_directories (walls-of-doom-source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
