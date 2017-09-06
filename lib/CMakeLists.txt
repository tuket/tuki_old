cmake_minimum_required(VERSION 2.8)

set(PROJ_NAME "tuki_lib")
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

set(SRC_RENDER_GL
	"mesh_gpu.hpp" "mesh_gpu.cpp"
	"attribs.hpp" "attribs.cpp"
	"attrib_initializers.hpp" "attrib_initializers.cpp"
	"shader.hpp" "shader.cpp"
	"texture.hpp" "texture.cpp"
	"render_target.hpp" "render_target.cpp"
	"util.hpp" "util.cpp"
	"render.hpp" "render.cpp"
)

set(SRC_RENDER_MATERIAL
	"material.hpp" "material.cpp"
)

set(SRC_RENDER_MESH
	"mesh.hpp" "mesh.cpp"
	"simple_meshes.hpp" "simple_meshes.cpp"
)

set(SRC_UTIL
	"util.hpp" "util.cpp"
)

# ----------------------------------------------------
# ADD _LIBRARY
# ----------------------------------------------------

PREPEND(SRC_RENDER_GL "src/tuki/render/gl" ${SRC_RENDER_GL})
PREPEND(SRC_RENDER_MATERIAL "src/tuki/render/material" ${SRC_RENDER_MATERIAL})
PREPEND(SRC_RENDER_MESH "src/tuki/render/mesh" ${SRC_RENDER_MESH})
PREPEND(SRC_UTIL "src/tuki/util" ${SRC_UTIL})

add_library(${PROJ_NAME}
	${SRC_RENDER_GL}
	${SRC_RENDER_MATERIAL}
	${SRC_RENDER_MESH}
	${SRC_UTIL}
)

# ----------------------------------------------------
# LINK LIBRARIES
# ----------------------------------------------------
set(LINK_LIBS
	"SDL2-static" "SDL2main"
	"pugixml"
	"stbi"
	"glad"
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

source_group("render\\gl" FILES ${SRC_RENDER_GL})
source_group("render\\material" FILES ${SRC_RENDER_MATERIAL})
source_group("render\\mesh" FILES ${SRC_RENDER_MESH})
source_group("util" FILES ${SRC_UTIL})

# ----------------------------------------------------
# Copy assets to the build directory
# ----------------------------------------------------
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:${PROJ_NAME}>
)