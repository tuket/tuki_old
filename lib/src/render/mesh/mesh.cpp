#include "mesh.hpp"

#include <glad/glad.h>
#include <stdio.h>
#include <cstring>

// upload vertex attrib data and set the pointer
inline void setVertexAttrib(
	int attribLocation, int vbo, unsigned numVerts, unsigned numDims, void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (data != nullptr)
	{
		glBufferData(GL_ARRAY_BUFFER, numVerts * numDims * sizeof(float),
			data, GL_STATIC_DRAW);
		glVertexAttribPointer(attribLocation, numDims, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(attribLocation);
	}
	else
	{
		glDisableVertexAttribArray(attribLocation);
	}
}

void MeshGpu::load(const Mesh& mesh)
{
	glGenVertexArrays(1, (GLuint*)&vao);
	const unsigned vboSetCount = sizeof(VboSet) / sizeof(unsigned);
	glGenBuffers(vboSetCount, (GLuint*)&vboSet);

	glBindVertexArray(vao);

	const unsigned nv = mesh.numVertices;
	const unsigned nt = mesh.numTriangles;

	setVertexAttrib(
		(int)EMeshAttribLocation::pos,
		vboSet.pos, nv, 3, mesh.positions);
	setVertexAttrib(
		(int)EMeshAttribLocation::color,
		vboSet.color, nv, 3, mesh.colors);
	setVertexAttrib(
		(int)EMeshAttribLocation::texCoord,
		vboSet.texCoord, nv, 2, mesh.texCoords);
	setVertexAttrib(
		(int)EMeshAttribLocation::normal,
		vboSet.normal, nv, 3, mesh.normals);
	setVertexAttrib(
		(int)EMeshAttribLocation::tangent,
		vboSet.tangent, nv, 3, mesh.tangents);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSet.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nt * 3 * sizeof(unsigned), mesh.triangles, GL_STATIC_DRAW);
}

void MeshGpu::free()
{
	freeVao(vao);
	freeVboSet(vboSet);
}

void freeVao(Vao vao)
{
	glDeleteBuffers(1, (GLuint*)&vao);
}

void freeVboSet(const VboSet& vboSet)
{
	glDeleteBuffers(sizeof(VboSet)/sizeof(GLuint), (GLuint*)&vboSet);
}

void Mesh::initVertexData
(
	unsigned numVertices,
	const float* positions,
	const float* normals,
	const float* tangents,
	const float* colors,
	const float* texCoords
)
{
	unsigned nv = numVertices;
	this->numVertices = nv;
	
	this->positions = new float[3 * nv];
	this->normals = new float[3 * nv];
	this->tangents = new float[3 * nv];
	this->colors = new float[3 * nv];
	this->texCoords = new float[2 * nv];
	memcpy(this->positions, positions, 3 * nv * sizeof(float));
	memcpy(this->normals, normals, 3 * nv * sizeof(float));
	memcpy(this->tangents, tangents, 3 * nv * sizeof(float));
	memcpy(this->colors, colors, 3 * nv * sizeof(float));
	memcpy(this->texCoords, texCoords, 2 * nv * sizeof(float));
}

void Mesh::initTrianglesData
(
	unsigned numTriangles,
	unsigned* triangles
)
{
	unsigned nt = numTriangles;
	this->numTriangles = nt;

	this->triangles = new unsigned[3 * nt];
	memcpy(this->triangles, triangles, 3 * nt * sizeof(unsigned));
}

void Mesh::free()
{
	delete[] positions;
	delete[] normals;
	delete[] tangents;
	delete[] colors;
	delete[] texCoords;
	delete[] triangles;
};