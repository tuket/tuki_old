#include "simple_meshes.hpp"
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>

using namespace std;
using namespace glm;

namespace SimpleMeshes
{

Mesh createHorizontalPlane(float width, float depth,
    unsigned widthDivisions, unsigned depthDivisions)
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

    mesh.initVertexData(nv, &positions[0][0], &normals[0][0], &tangents[0][0], &colors[0][0], &texCoords[0][0]);
    mesh.initTrianglesData(nt, &triangles[0]);

    return mesh;
}

UvPlaneMeshGpu createScreenUvPlane(float width, float height)
{
	UvPlaneMeshGpu mesh;
	mesh.load();
	return mesh;
}

Mesh createBox(float width, float height, float depth)
{
	const unsigned numQuads = 6;
	const unsigned nv = 4 * numQuads;
	const unsigned nt = 2 * numQuads;

	const float w = width / 2;
	const float h = height / 2;
	const float d = depth / 2;
	
	// positions
	vector<vec3> positions =
	{
		// face -x
		vec3(-w, -h, +d), vec3(-w, +h, +d), vec3(-w, +h, -d), vec3(-w, -h, -d),
		// face +x
		vec3(+w, -h, +d), vec3(+w, -h, -d), vec3(+w, +h, -d), vec3(+w, +h, +d),
		// face -y
		vec3(-w, -h, +d), vec3(-w, -h, -d), vec3(+w, -h, -d), vec3(+w, -h, +d),
		// face +y
		vec3(-w, +h, +d), vec3(+w, +h, +d), vec3(+w, +h, -d), vec3(-w, +h, -d),
		// face -z
		vec3(-w, -h, -d), vec3(-w, +h, -d), vec3(+w, +h, -d), vec3(+w, -h, -d),
		// face +z
		vec3(-w, -h, +d), vec3(+w, -h, +d), vec3(+w, +h, +d), vec3(-w, +h, +d)
	};

	// normals
	vector<vec3> normals =
	{
		// face -x
		vec3(-1, 0, 0), vec3(-1, 0, 0), vec3(-1, 0, 0), vec3(-1, 0, 0),
		// face +x
		vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0),
		// face -y
		vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0),
		// face +y
		vec3(0, +1, 0), vec3(0, +1, 0), vec3(0, +1, 0), vec3(0, +1, 0),
		// face -z
		vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1),
		// face +z
		vec3(0, 0, +1), vec3(0, 0, +1), vec3(0, 0, +1), vec3(0, 0, +1)
	};

    // tangents
    vector<vec3> tangents =
    {
        // face -x
		vec3(0, 0, +1), vec3(0, 0, +1), vec3(0, 0, +1), vec3(0, 0, +1),
		// face +x
		vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1),
		// face -y
		vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0),
		// face +y
		vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0),
		// face -z
		vec3(-1, 0, 0), vec3(-1, 0, 0), vec3(-1, 0, 0), vec3(-1, 0, 0),
		// face +z
		vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0), vec3(+1, 0, 0)
    };

    // colors
    vector<vec3> colors(nv);
    for(unsigned i=0; i<nv; i++) colors[i] = vec3(0.8f, 0.8f, 0.8f);

    // texCoords
    vector<vec2> texCoords =
    {
        // face -x
		vec2(1, 0), vec2(1, 1), vec2(0, 1), vec2(0, 0),
		// face +x
		vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1),
		// face -y
		vec2(0, 1), vec2(0, 0), vec2(1, 0), vec2(1, 1),
		// face +y
		vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1),
		// face -z
		vec2(1, 0), vec2(1, 1), vec2(0, 1), vec2(0, 0),
		// face +z
		vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1)
    };

    // triangles
	vector<unsigned> triangles =
	{
		// face -x
		0, 1, 2,
		0, 2, 3,
		// face +x
		4, 5, 6,
		4, 6, 7,
		// face -y
		8, 9, 10,
		8, 10, 11,
		// face +y
		12, 13, 14,
		12, 14, 15,
		// face -z
		16, 17, 18,
		16, 18, 19,
		// face +z
		20, 21, 22,
		20, 22, 23
	};

    Mesh mesh;
    mesh.initVertexData(nv, &positions[0][0], &normals[0][0], &tangents[0][0], &colors[0][0], &texCoords[0][0]);
    mesh.initTrianglesData(nt, &triangles[0]);
    return mesh;
}

}