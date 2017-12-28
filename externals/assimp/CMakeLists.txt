# Open Asset Import Library (assimp)
# ----------------------------------------------------------------------
# Copyright (c) 2006-2017, assimp team
# All rights reserved.
#
# Redistribution and use of this software in source and binary forms,
# with or without modification, are permitted provided that the
# following conditions are met:
#
# * Redistributions of source code must retain the above
#   copyright notice, this list of conditions and the
#   following disclaimer.
#
# * Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the
#   following disclaimer in the documentation and/or other
#   materials provided with the distribution.
#
# * Neither the name of the assimp team, nor the names of its
#   contributors may be used to endorse or promote products
#   derived from this software without specific prior
#   written permission of the assimp team.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#----------------------------------------------------------------------
SET(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required( VERSION 2.8 )
PROJECT( Assimp )

# All supported options ###############################################
OPTION( ASSIMP_DOUBLE_PRECISION
  "Set to ON to enable double precision processing"
  OFF
)
OPTION( ASSIMP_NO_EXPORT
  "Disable Assimp's export functionality."
  OFF
)

if (WIN32)
    add_definitions( -DWIN32_LEAN_AND_MEAN )
endif()

IF(MSVC)
  set (CMAKE_PREFIX_PATH "D:\\libs\\devil")
  OPTION( ASSIMP_INSTALL_PDB
    "Install MSVC debug files."
    ON
  )
ENDIF(MSVC)

SET(LINK_SEARCH_START_STATIC TRUE)

# Define here the needed parameters
SET (ASSIMP_VERSION_MAJOR 4)
SET (ASSIMP_VERSION_MINOR 0)
SET (ASSIMP_VERSION_PATCH 1) # subversion revision?
SET (ASSIMP_VERSION ${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH})
SET (ASSIMP_SOVERSION 4)
SET (PROJECT_VERSION "${ASSIMP_VERSION}")

SET(ASSIMP_PACKAGE_VERSION "0" CACHE STRING "the package-specific version used for uploading the sources")

# Get the current working branch
EXECUTE_PROCESS(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

# Get the latest abbreviated commit hash of the working branch
EXECUTE_PROCESS(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

IF(NOT GIT_COMMIT_HASH)
  SET(GIT_COMMIT_HASH 0)
ENDIF(NOT GIT_COMMIT_HASH)

IF(ASSIMP_DOUBLE_PRECISION)
  ADD_DEFINITIONS(-DASSIMP_DOUBLE_PRECISION)
ENDIF(ASSIMP_DOUBLE_PRECISION)

# Check for OpenMP support
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(
    ./
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules" )
SET(LIBASSIMP_COMPONENT     "libassimp${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH}" )
SET(LIBASSIMP-DEV_COMPONENT "libassimp${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH}-dev" )
SET(CPACK_COMPONENTS_ALL assimp-bin ${LIBASSIMP_COMPONENT} ${LIBASSIMP-DEV_COMPONENT} assimp-dev)
SET(ASSIMP_LIBRARY_SUFFIX "" CACHE STRING "Suffix to append to library names")

IF( UNIX )
  # Ensure that we do not run into issues like http://www.tcm.phy.cam.ac.uk/sw/inodes64.html on 32 bit linux
  IF ( CMAKE_SIZEOF_VOID_P EQUAL 4) # only necessary for 32-bit linux
    ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 )
  ENDIF()

  # Use GNUInstallDirs for Unix predefined directories
  include(GNUInstallDirs)
ENDIF( UNIX )


# Grouped compiler settings
IF ((CMAKE_C_COMPILER_ID MATCHES "GNU") AND NOT CMAKE_COMPILER_IS_MINGW)
  # hide all not-exported symbols
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fvisibility=hidden -fPIC -Wall -std=c++0x")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  SET(LIBSTDC++_LIBRARIES -lstdc++)
ELSEIF(MSVC)
  # enable multi-core compilation with MSVC
  add_compile_options(/MP)
ELSEIF ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fvisibility=hidden -fPIC -Wall -Wno-long-long -pedantic -std=c++11" )
ELSEIF( CMAKE_COMPILER_IS_MINGW )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -Wno-long-long -pedantic -std=c++11" )
  add_definitions( -U__STRICT_ANSI__ )
ENDIF()

INCLUDE_DIRECTORIES( "include" )

# If this is an in-source build (CMAKE_SOURCE_DIR == CMAKE_BINARY_DIR),
# write the library/executable files to the respective directories in the
# source tree. During an out-of-source build, however, do not litter this
# directory, since that is probably what the user wanted to avoid.
IF ( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
  SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/lib" )
  SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/lib" )
  SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin" )
ENDIF ( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )

message(STATUS "compiling zlib from souces")
include(CheckTypeSize)
include(CheckFunctionExists)
# compile from sources
add_subdirectory(contrib/zlib)
SET(ZLIB_FOUND 1)
SET(ZLIB_LIBRARIES zlibstatic)
SET(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contrib/zlib ${CMAKE_CURRENT_BINARY_DIR}/contrib/zlib)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

IF ( ASSIMP_NO_EXPORT )
  ADD_DEFINITIONS( -DASSIMP_BUILD_NO_EXPORT)
  MESSAGE( STATUS "Build an import-only version of Assimp." )
ENDIF( ASSIMP_NO_EXPORT )

SET ( ASSIMP_BUILD_ARCHITECTURE "" CACHE STRING
  "describe the current architecture."
)
IF    ( ASSIMP_BUILD_ARCHITECTURE STREQUAL "")
ELSE  ( ASSIMP_BUILD_ARCHITECTURE STREQUAL "")
  ADD_DEFINITIONS ( -D'ASSIMP_BUILD_ARCHITECTURE="${ASSIMP_BUILD_ARCHITECTURE}"' )
ENDIF ( ASSIMP_BUILD_ARCHITECTURE STREQUAL "")

# ${CMAKE_GENERATOR}
SET ( ASSIMP_BUILD_COMPILER "" CACHE STRING
  "describe the current compiler."
)
IF    ( ASSIMP_BUILD_COMPILER STREQUAL "")
ELSE  ( ASSIMP_BUILD_COMPILER STREQUAL "")
  ADD_DEFINITIONS ( -D'ASSIMP_BUILD_COMPILER="${ASSIMP_BUILD_COMPILER}"' )
ENDIF ( ASSIMP_BUILD_COMPILER STREQUAL "")

MARK_AS_ADVANCED ( ASSIMP_BUILD_ARCHITECTURE ASSIMP_BUILD_COMPILER )

ADD_SUBDIRECTORY(contrib)

ADD_SUBDIRECTORY( code/ )
