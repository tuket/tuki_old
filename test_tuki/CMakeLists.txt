cmake_minimum_required(VERSION 2.8)

set(PROJ_NAME "test_tuki")
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

add_executable("test1"
	"test1.cpp"
)

add_executable("test2"
	"test2.cpp"
)

target_link_libraries("test1" "tuki_lib")
target_link_libraries("test2" "tuki_lib")

# ----------------------------------------------------
# Copy assets to the build directory
# ----------------------------------------------------
add_custom_target("copy_shaders")
add_custom_command(TARGET "copy_shaders" POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/shaders
	$<TARGET_FILE_DIR:test1>/shaders
)