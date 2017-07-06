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


set(INCLUDE_DIRS
	"externals/SDL2/include"
	"externals/pugixml/src"
	"externals/glm/"
	"externals/stbi"
	"externals/glad/include"
	"externals/assimp/include"
)
include_directories(${INCLUDE_DIRS})


# this function preppends a path to all files in a list
FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)


set(SRC_PATH "src/")

# copy assets to the build directory
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:${PROJ_NAME}>
