cmake_minimum_required( VERSION 3.21 )
set( CMAKE_CXX_STANDARD 17 )
project( destructive_drones )

set( BUILD_STATIC_LIBS ON )
add_subdirectory( ext/raylib )
add_subdirectory( ext/glm )

add_executable( destructive_drones src/main.cpp )
target_link_libraries( destructive_drones PUBLIC raylib glm )

if (EMSCRIPTEN)
	set_target_properties( destructive_drones PROPERTIES COMPILE_FLAGS "-s ASYNCIFY" )
	set_target_properties( destructive_drones PROPERTIES LINK_FLAGS "-s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/build@/ --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html" )
	set_target_properties( destructive_drones PROPERTIES OUTPUT_NAME "index" )
	set_target_properties( destructive_drones PROPERTIES SUFFIX ".html" )
endif ()
