#pragma once

#include "mesh.hpp"

namespace SimpleMeshes
{

Mesh createHorizontalPlane(float width = 1, float depth = 1,
unsigned widthDivisions = 1, unsigned depthDivisions = 1);

UvPlaneMeshGpu createScreenUvPlane(float width, float height);

Mesh createBox(float width = 1, float height = 1, float depth = 1);

}
