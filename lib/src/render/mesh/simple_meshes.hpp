#ifndef SIMPLE_MESHES_HPP
#define SIMPLE_MESHES_HPP

#include "mesh.hpp"

namespace SimpleMeshes
{

Mesh createHorizontalPlane(float width = 1, float depth = 1,
unsigned widthDivisions = 1, unsigned depthDivisions = 1);

void createScreenUvPlane(unsigned& vao, unsigned& vbo, float width, float height);

}
#endif