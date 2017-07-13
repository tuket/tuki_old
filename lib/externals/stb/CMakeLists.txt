project(stb)

cmake_minimum_required(VERSION 2.6)

set(SOURCES stbi.h stbi.c)

add_library(stbi STATIC ${SOURCES})