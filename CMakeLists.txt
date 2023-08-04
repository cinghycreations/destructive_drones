cmake_minimum_required( VERSION 3.21 )
set( CMAKE_CXX_STANDARD 17 )
project( destructive_drones )

set( BUILD_STATIC_LIBS ON )
add_subdirectory( ext/raylib )
add_subdirectory( ext/glm )

add_executable( destructive_drones src/main.cpp )
target_link_libraries( destructive_drones PUBLIC raylib glm )
