cmake_minimum_required (VERSION 2.8.7)

set (CMAKE_MODULE_PATH ../cmake)

if (COMPILER_IS_GNU_OR_CLANG)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS_BASE} -std=c89")
endif ()

find_package (SDL2 REQUIRED)
find_package (SDL2_ttf REQUIRED)
find_package (SDL2_image REQUIRED)
include_directories (${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable (tests tests.c)
target_link_libraries (tests unity)
target_link_libraries (tests walls-of-doom-base)
target_link_libraries (tests ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

# Copy the assets to the binary directory.
add_custom_command (TARGET tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/)
