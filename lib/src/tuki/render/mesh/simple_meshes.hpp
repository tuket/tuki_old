#pragma once

#include "mesh.hpp"

#include "../gl/mesh_gpu.hpp"

namespace SimpleMeshes
{

Mesh createHorizontalPlane(float width = 1, float depth = 1,
unsigned widthDivisions = 1, unsigned depthDivisions = 1);

Mesh createBox(float width = 1, float height = 1, float depth = 1);

}
