#include "simple_meshes.hpp"
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace std;
using namespace glm;

namespace SimpleMeshes
{


Mesh createHorizontalPlane(float width = 1, float depth = 1,
    unsigned widthDivisions = 1, unsigned depthDivisions = 1)
{
    Mesh mesh;

    const unsigned wd = widthDivisions;
    const unsigned dd = depthDivisions;
    const  unsigned nv = (wd + 1) * (dd + 1);   // num vertices
    const unsigned nq = wd * dd;    // num quads
    const unsigned nt = 2 * nq;     // num triangles

    // positions
    vector<vec3> positions;
    positions.reserve(nv);
    for(int iz; iz < dd+1; iz++)
    for(int ix; ix < wd+1; ix++)
    {
        float x = - (width / 2) + ((float)ix / wd) * width;
        float z = + (depth / 2) - ((float)iz / dd) * depth;
        vec3 pos(x, 0, z);
        positions.push_back(pos);
    }

    // normals
    vector<vec3> normals;
    normals.reserve(nv);
    for(int i=0; i < nv; i++) normals[i] = vec3(0, 1, 0);

    // 
}

}