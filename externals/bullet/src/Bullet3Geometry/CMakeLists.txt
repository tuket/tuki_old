
INCLUDE_DIRECTORIES(
	"../"
)

SET(Bullet3Geometry_SRCS
	b3ConvexHullComputer.cpp
	b3GeometryUtil.cpp
)

SET(Bullet3Geometry_HDRS
	b3AabbUtil.h
	b3ConvexHullComputer.h
	b3GeometryUtil.h
	b3GrahamScan2dConvexHull.h
)

ADD_LIBRARY(Bullet3Geometry ${Bullet3Geometry_SRCS} ${Bullet3Geometry_HDRS})
if (BUILD_SHARED_LIBS)
  target_link_libraries(Bullet3Geometry Bullet3Common)
endif()