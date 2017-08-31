cmake_minimum_required(VERSION 2.6)

project(glad)

include_directories(include)

add_library(glad STATIC include/glad/glad.h src/glad.c)

if(UNIX)
	target_link_libraries(glad m dl pthread)
endif()