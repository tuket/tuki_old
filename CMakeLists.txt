cmake_minimum_required(VERSION 3.1)

set(PROJ_NAME "tuki")
project(${PROJ_NAME})

set (CMAKE_CXX_STANDARD 11)

# this allows to have big obj files linked in Debug build with VS
if(MSVC)
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/bigobj")
endif()


# ----------------------------------------------------
# Externals
# ----------------------------------------------------

# OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
	link_libraries(${OPENGL_LIBRARIES})
endif()

# SDL2
add_subdirectory("externals/SDL2")

# stb
add_subdirectory("externals/stb")

# glad
add_subdirectory("externals/glad")

# pugixml
add_subdirectory("externals/pugixml")

# assimp
OPTION(ASSIMP_BUILD_ZLIB ON)
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
option(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory("externals/assimp")

# bullet
OPTION(BUILD_BULLET3 "Set when you want to build Bullet 3" OFF)
add_subdirectory("externals/bullet")


set(INCLUDE_DIRS
	"externals/SDL2/include"
	"externals/pugixml/src"
	"externals/glm/"
	"externals/stb"
	"externals/glad/include"
	"externals/assimp/include"
	"externals/rapidjson"
	"externals/cereal"
	"externals/bullet/src"
	
	"lib"
)
include_directories(${INCLUDE_DIRS})

# ----------------------------------------------------
# Subprojects
# ----------------------------------------------------

add_subdirectory("lib")
add_subdirectory("test_tuki")
#add_subdirectory("editor")
