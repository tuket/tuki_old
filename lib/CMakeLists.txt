cmake_minimum_required(VERSION 2.8)

set(PROJ_NAME tuki_lib)
project(${PROJ_NAME})

# this function preppends a path to all files in a list
FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

# ----------------------------------------------------
# SOURCE FILES LIST
# ----------------------------------------------------

set(SRC_RENDER_MESH
	"mesh.hpp" "mesh.cpp"
	"simple_meshes.hpp" "simple_meshes.cpp"
)

set(SRC_RENDER_SHADER
	"shader.hpp" "shader.cpp"
)

set(SRC_UTIL
	"util.hpp" "util.cpp"
)

# ----------------------------------------------------
# ADD _LIBRARY
# ----------------------------------------------------

PREPEND(SRC_RENDER_MESH "src/render/mesh" ${SRC_RENDER_MESH})
PREPEND(SRC_RENDER_SHADER "src/render/shader" ${SRC_RENDER_SHADER})
PREPEND(SRC_UTIL "src/util" ${SRC_UTIL})

add_library(${PROJ_NAME}
	${SRC_RENDER_MESH}
	${SRC_RENDER_SHADER}
	${SRC_UTIL}
)

# ----------------------------------------------------
# LINK LIBRARIES
# ----------------------------------------------------
set(LINK_LIBS
	"SDL2" "SDL2main"
	"pugixml"
	"stbi"
	"glad"
	"zlibstatic"
	"assimp"
)
if(UNIX)
	set(LINK_LIBS ${LINK_LIBS} "m" "dl" "pthread")
endif()
target_link_libraries(${PROJ_NAME} ${LINK_LIBS})

# ----------------------------------------------------
# Target include directories
# ----------------------------------------------------
target_include_directories(${PROJ_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${INCLUDE_DIRS}
)

# ----------------------------------------------------
# Source groups
# ----------------------------------------------------

source_group("render\\mesh" FILES ${SRC_RENDER_MESH})
source_group("render\\shader" FILES ${SRC_RENDER_SHADER})
source_group("util" FILES ${SRC_UTIL})

# ----------------------------------------------------
# Copy assets to the build directory
# ----------------------------------------------------
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:${PROJ_NAME}>
)