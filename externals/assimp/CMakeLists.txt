cmake_minimum_required( VERSION 2.8 )
PROJECT( Assimp )

OPTION(BUILD_SHARED_LIBS "Build package with shared libraries." ON)
IF(NOT BUILD_SHARED_LIBS)
  SET(LINK_SEARCH_START_STATIC TRUE)
ENDIF(NOT BUILD_SHARED_LIBS)

OPTION ( ASSIMP_NO_EXPORT
  "Disable Assimp's export functionality."
  OFF
)

IF ( ASSIMP_NO_EXPORT )
  ADD_DEFINITIONS( -DASSIMP_BUILD_NO_EXPORT)
  MESSAGE( STATUS "Build an import-only version of Assimp." )
ENDIF( ASSIMP_NO_EXPORT )

ADD_SUBDIRECTORY( code/ )
