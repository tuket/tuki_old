cmake_minimum_required(VERSION 2.8)

set(PROJ_NAME tuki)
project(${PROJ_NAME})

# try to use C++ 11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++0x support. Please use a different C++ compiler.")
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

# zlib
add_subdirectory("externals/zlib")

# assimp
option(ASSIMP_BUILD_ASSIMP_TOOLS ON)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
option(ASSIMP_BUILD_STATIC_LIB ON)
add_subdirectory("externals/assimp")


set(INCLUDE_DIRS
	"externals/SDL2/include"
	"externals/pugixml/src"
	"externals/glm/"
	"externals/stbi"
	"externals/glad/include"
	"externals/assimp/include"
)
include_directories(${INCLUDE_DIRS})

# ----------------------------------------------------
# Subprojects
# ----------------------------------------------------

add_subdirectory("lib")
#add_subdirectory("editor")