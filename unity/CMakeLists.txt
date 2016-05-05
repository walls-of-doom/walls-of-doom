cmake_minimum_required (VERSION 2.8.7)

add_library (unity unity.c unity.h unity_internals.h)

target_include_directories (unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
