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
	"frame_buffer.hpp" "frame_buffer.cpp"
	"render_target.hpp" "render_target.cpp"
	"util.hpp" "util.cpp"
	"render.hpp" "render.cpp"
)

set(SRC_RENDER_MATERIAL
	"material.hpp" "material.cpp"
	"shader_pool.hpp" "shader_pool.cpp"
)

set(SRC_RENDER_MESH
	"mesh.hpp" "mesh.cpp"
	"simple_meshes.hpp" "simple_meshes.cpp"
)

set(SRC_UTIL
	"util.hpp" "util.cpp"
	"singleton.hpp"
	"multi_sort.hpp"
)

set(SRC_ENGINE_SCENE
	"scene.hpp" "scene.cpp"
	"scene_node.hpp" "scene_node.cpp"
)

set(SRC_ENGINE_SCENE_COMPONENT
	"component.hpp"
	"mesh_component.hpp"
	"material_component.hpp"
	"point_light_component.hpp"
)

set(SRC_ENGINE_GRAPHICS
	"graphics_system.hpp"
	"renderer_3d.hpp"
)

# ----------------------------------------------------
# ADD _LIBRARY
# ----------------------------------------------------

PREPEND(SRC_RENDER_GL "src/tuki/render/gl" ${SRC_RENDER_GL})
PREPEND(SRC_RENDER_MATERIAL "src/tuki/render/material" ${SRC_RENDER_MATERIAL})
PREPEND(SRC_RENDER_MESH "src/tuki/render/mesh" ${SRC_RENDER_MESH})
PREPEND(SRC_UTIL "src/tuki/util" ${SRC_UTIL})
PREPEND(SRC_ENGINE_SCENE "src/tuki/engine/scene" ${SRC_ENGINE_SCENE})
PREPEND(SRC_ENGINE_SCENE_COMPONENT "src/tuki/engine/scene/component" ${SRC_ENGINE_SCENE_COMPONENT})
PREPEND(SRC_ENGINE_GRAPHICS "src/tuki/engine/graphics" ${SRC_ENGINE_GRAPHICS})

add_library(${PROJ_NAME}
	${SRC_RENDER_GL}
	${SRC_RENDER_MATERIAL}
	${SRC_RENDER_MESH}
	${SRC_UTIL}
	${SRC_ENGINE_SCENE}
	${SRC_ENGINE_SCENE_COMPONENT}
	${SRC_ENGINE_GRAPHICS}
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
source_group("engine\\scene" FILES ${SRC_ENGINE_SCENE})
source_group("engine\\scene\\component" FILES ${SRC_ENGINE_SCENE_COMPONENT})
source_group("engine\\graphics" FILES ${SRC_ENGINE_GRAPHICS})


# ----------------------------------------------------
# Copy assets to the build directory
# ----------------------------------------------------
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:${PROJ_NAME}>
)