cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS_BASE}")

add_library (unity unity.c unity.h unity_internals.h)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
