#include "mesh.hpp"

#include <stdio.h>
#include <cstring>
#include <cassert>

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

const float* Mesh::getAttribData(AttribLocation index)const
{
	const float* dataPtr = nullptr;
	switch (index)
	{
		case AttribLocation::POS:		 dataPtr = positions; break;
		case AttribLocation::NORMAL:	 dataPtr = normals; break;
		case AttribLocation::TANGENT:	 dataPtr = tangents; break;
		case AttribLocation::COLOR:	 dataPtr = colors; break;
		case AttribLocation::TEX_COORD: dataPtr = texCoords; break;
	}
	return dataPtr;
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