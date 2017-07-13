project(glad)

cmake_minimum_required(VERSION 2.6)

include_directories("include")

set(SOURCES include/glad/glad.h src/glad.c)

add_library(glad STATIC ${SOURCES})

if(UNIX)
	target_link_libraries(glad "m" "dl" "pthread")
endif()