#include "mesh.hpp"

#include <glad/glad.h>
#include <stdio.h>
#include <cstring>
#include <cassert>

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

void MeshGpuGeneric::load(const IMesh& mesh)
{
	glGenVertexArrays(1, (GLuint*)&vao);
	const unsigned vboSetCount = sizeof(VboSetFull) / sizeof(int);
	glGenBuffers(vboSetCount, (GLuint*)&vboSet);

	glBindVertexArray(vao);

	const unsigned nv = mesh.getNumVertices();
	const unsigned ni = mesh.getNumIndices();

	attribBitMask = EAttribBitMask::NONE;

	for (unsigned i = 0; i < (unsigned)EAttribLocation::NUM_ATTRIBS; i++)
	{
		EAttribLocation curAttrib = (EAttribLocation)i;
		if (mesh.hasAttribData((EAttribLocation)i))
		{
			attribBitMask |= (EAttribBitMask)(1 << i);
			setVertexAttrib(
				i, vboSet.attribs[i], ////////// CURRENT CODING POINT
			);
		}
	}
}

void MeshGpuGeneric::load(const IMesh& mesh)
{

}

void MeshGpuGeneric::load(const Mesh& mesh)
{
	glGenVertexArrays(1, (GLuint*)&vao);
	const unsigned vboSetCount = sizeof(VboSet) / sizeof(unsigned);
	glGenBuffers(vboSetCount, (GLuint*)&vboSet);

	glBindVertexArray(vao);

	const unsigned nv = mesh.numVertices;
	const unsigned nt = mesh.numTriangles;

	attribBitMask = EAttribBitMask::NONE;

	if (mesh.positions)
	{
		attribBitMask |= EAttribBitMask::POS;
		setVertexAttrib(
			(int)EAttribLocation::POS,
			vboSet.pos, nv, 3, mesh.positions);
		glEnableVertexAttribArray((unsigned)EAttribLocation::POS);
	}
	if (mesh.colors)
	{
		attribBitMask |= EAttribBitMask::COLOR;
		setVertexAttrib(
			(int)EAttribLocation::COLOR,
			vboSet.color, nv, 3, mesh.colors);
		glEnableVertexAttribArray((unsigned)EAttribLocation::COLOR);
	}
	if (mesh.texCoords)
	{
		attribBitMask |= EAttribBitMask::TEX_COORD;
		setVertexAttrib(
			(int)EAttribLocation::TEX_COORD,
			vboSet.texCoord, nv, 2, mesh.texCoords);
		glEnableVertexAttribArray((unsigned)EAttribLocation::TEX_COORD);
	}
	if (mesh.normals)
	{
		attribBitMask |= EAttribBitMask::NORMAL;
		setVertexAttrib(
			(int)EAttribLocation::NORMAL,
			vboSet.normal, nv, 3, mesh.normals);
		glEnableVertexAttribArray((unsigned)EAttribLocation::NORMAL);
	}
	if (mesh.tangents)
	{
		attribBitMask |= EAttribBitMask::TANGENT;
		setVertexAttrib(
			(int)EAttribLocation::TANGENT,
			vboSet.tangent, nv, 3, mesh.tangents);
		glEnableVertexAttribArray((unsigned)EAttribLocation::TANGENT);
	}

	assert(mesh.triangles);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSet.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nt * 3 * sizeof(unsigned), mesh.triangles, GL_STATIC_DRAW);
}

void MeshGpuGeneric::free()
{
	freeVao(vao);
	freeVboSet(vboSet);
}

void freeVao(Vao vao)
{
	glDeleteBuffers(1, (GLuint*)&vao);
}

void freeVbo(Vbo vbo)
{
	glDeleteBuffers(1, (GLuint*)&vbo);
}

void freeVaos(const Vao* vaos, unsigned num)
{
	glDeleteBuffers(num, (const GLuint*)vaos);
}

void freeVbos(const Vbo* vbos, unsigned num)
{
	glDeleteBuffers(num, (const GLuint*)vbos);
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

const float* Mesh::getAttribData(EAttribLocation index)const
{
	const float* dataPtr = nullptr;
	switch (index)
	{
		case EAttribLocation::POS:		 dataPtr = positions; break;
		case EAttribLocation::NORMAL:	 dataPtr = normals; break;
		case EAttribLocation::TANGENT:	 dataPtr = tangents; break;
		case EAttribLocation::COLOR:	 dataPtr = colors; break;
		case EAttribLocation::TEX_COORD: dataPtr = texCoords; break;
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