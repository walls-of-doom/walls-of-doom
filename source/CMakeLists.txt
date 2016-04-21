cmake_minimum_required (VERSION 2.9)

add_library (walls-of-doom-source insertion-sort.h array-utils.h game-logic.h)

set_target_properties (walls-of-doom-source PROPERTIES LINKER_LANGUAGE C)

target_include_directories (walls-of-doom-source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
