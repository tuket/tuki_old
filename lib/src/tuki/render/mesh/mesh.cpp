#include "mesh.hpp"

#include <stdio.h>
#include <cstring>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

using namespace std;

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
		case AttribLocation::COLOR:		 dataPtr = colors; break;
		case AttribLocation::TEX_COORD:  dataPtr = texCoords; break;
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

void Mesh::initFromFile(const string& fileName)
{
	Assimp::Importer importer;
	
	const aiScene* scene =
		importer.ReadFile(fileName,
			aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		throw runtime_error("could not load " + fileName);
	}

	if (!scene->HasMeshes())
	{
		throw runtime_error("there are no meshes in " + fileName);
	}

	aiMesh* mesh = scene->mMeshes[0];
	const unsigned nv = mesh->mNumVertices;
	const unsigned nt = mesh->mNumFaces;

	if (!mesh->HasPositions())
	{
		throw runtime_error("the mesh doesn't have any positions (" + fileName + ")");
	}
	
	numVertices = nv;
	numTriangles = nt;
	
	positions = new float[3 * nv];
	for (unsigned i = 0; i < nv; i++)
	for (unsigned j = 0; j < 3; j++)
	{
		positions[3 * i + j] = mesh->mVertices[i][j];
	}

	normals = nullptr;
	if (mesh->HasNormals())
	{
		normals = new float[3 * nv];
		for (unsigned i = 0; i < nv; i++)
		for (unsigned j = 0; j < 3; j++)
		{
			normals[3 * i + j] = mesh->mNormals[i][j];
		}
	}

	tangents = nullptr;
	if (mesh->HasTangentsAndBitangents())
	{
		tangents = new float[3 * nv];
		for (unsigned i = 0; i < nv; i++)
		for (unsigned j = 0; j < 3; j++)
		{
			tangents[3 * i + j] = mesh->mTangents[i][j];
		}
	}

	colors = nullptr;
	if (mesh->HasVertexColors(0))
	{
		colors = new float[3 * nv];
		for (unsigned i = 0; i < nv; i++)
		for (unsigned j = 0; j < 3; j++)
		{
			colors[3 * i + j] = mesh->mColors[0][i][j];
		}
	}

	texCoords = nullptr;
	if (mesh->HasTextureCoords(0))
	{
		texCoords = new float[2 * nv];
		for (unsigned i = 0; i < nv; i++)
		for (unsigned j = 0; j < 2; j++)
		{
			texCoords[2 * i + j] = mesh->mTextureCoords[0][i][j];
		}
	}

	triangles = nullptr;
	if (!mesh->HasFaces())
	{
		throw runtime_error("mesh doesn't have faces(" + fileName + ")");
	}
	triangles = new unsigned[3 * nt];
	for (unsigned i = 0; i < nt; i++)
	for (unsigned j = 0; j < 3; j++)
	{
		triangles[3 * i + j] = mesh->mFaces[i].mIndices[j];
	}
}