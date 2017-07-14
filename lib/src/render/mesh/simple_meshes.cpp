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
    vector<vec3> normals(nv);
    for(int i=0; i < nv; i++) normals[i] = vec3(0, 1, 0);

    // tangents
    vector<vec3> tangents(nv);
    for(int i=0; i < nv; i++) tangents[i] = vec3(1, 0, 0);

    // colors
    vector<vec3> colors(nv);
    for(int i=0; i < nv; i++) colors[i] = vec3(0.8f, 0.8f, 0.8f);

    // texCoords
    vector<vec2> texCoords;
    texCoords.reserve(nv);
    for(int iz; iz < dd+1; iz++)
    for(int ix; ix < wd+1; ix++)
    {
        float u = (float)ix / wd;
        float v = (float)iz / dd;
        vec2 uv(u, v);
        texCoords.push_back(uv);
    }

    // triangles
    vector<unsigned> triangles;
    triangles.reserve(nt * 3);
    for(unsigned iz=0; iz < dd; iz++)
    for(unsigned ix=0; ix < wd; ix++)
    {
        unsigned i00 = ix + iz * (wd + 1);
        unsigned i01 = (ix + 1) + iz * (wd + 1);
        unsigned i10 = ix + (iz + 1) * (wd + 1);
        unsigned i11 = (ix + 1) + (iz + 1) * (wd + 1);
        triangles.push_back(i00); triangles.push_back(i01); triangles.push_back(i11);
        triangles.push_back(i00); triangles.push_back(i11); triangles.push_back(i10);
    }

    mesh.initVertexData(nv, &positions[0][0]), &normals[0][0], &tagents[0][0], &colors[0][0], &texCoord[0][0]);
    mesh.initTrianglesData(nt, &triangles[0]);

    return mesh;
}

void createScreenUvPlane(unsigned& vao, unsigned& vbo, float width, float height)
{
    const unsigned numVerts = 4;
    const float planeUVCoords[numVerts] =
    {
        0, 0,
        1, 0,
        0, 1,
        1, 1
    };

    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(float) * 2,
		planeUVCoords, GL_STATIC_DRAW);
	glVertexAttribPointer((unsigned)EMeshAttribLocation::texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray((unsigned)EMeshAttribLocation::texCoord);
}

}