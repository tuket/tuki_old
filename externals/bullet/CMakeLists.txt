
IF(BUILD_BULLET3)
	SUBDIRS(
	src/Bullet3OpenCL
	src/Bullet3Serialize/Bullet2FileLoader
	src/Bullet3Dynamics
	src/Bullet3Collision
	src/Bullet3Geometry )
ENDIF(BUILD_BULLET3)


SUBDIRS(
src/BulletInverseDynamics
src/BulletSoftBody
src/BulletCollision
src/BulletDynamics
src/LinearMath
src/Bullet3Common)