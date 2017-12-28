
INCLUDE_DIRECTORIES(
	"../"
)

#SUBDIRS( Solvers )

SET(BulletSoftBody_SRCS
	btSoftBody.cpp
	btSoftBodyConcaveCollisionAlgorithm.cpp
	btSoftBodyHelpers.cpp
	btSoftBodyRigidBodyCollisionConfiguration.cpp
	btSoftRigidCollisionAlgorithm.cpp
	btSoftRigidDynamicsWorld.cpp
	btSoftMultiBodyDynamicsWorld.cpp
	btSoftSoftCollisionAlgorithm.cpp
	btDefaultSoftBodySolver.cpp

)

SET(BulletSoftBody_HDRS
	btSoftBody.h
	btSoftBodyData.h
	btSoftBodyConcaveCollisionAlgorithm.h
	btSoftBodyHelpers.h
	btSoftBodyRigidBodyCollisionConfiguration.h
	btSoftRigidCollisionAlgorithm.h
	btSoftRigidDynamicsWorld.h
	btSoftMultiBodyDynamicsWorld.h
	btSoftSoftCollisionAlgorithm.h
	btSparseSDF.h

	btSoftBodySolvers.h
	btDefaultSoftBodySolver.h

	btSoftBodySolverVertexBuffer.h
)



ADD_LIBRARY(BulletSoftBody  ${BulletSoftBody_SRCS} ${BulletSoftBody_HDRS})
IF (BUILD_SHARED_LIBS)
	TARGET_LINK_LIBRARIES(BulletSoftBody BulletDynamics)
ENDIF (BUILD_SHARED_LIBS)