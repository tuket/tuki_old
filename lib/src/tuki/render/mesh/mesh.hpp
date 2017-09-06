#pragma once

#include "../gl/attribs.hpp"

enum class GeomType
{
	POINTS,
	LINES,
	TRIANGLES,
	LINES_STRIP,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,

	COUNT
};

// MESH INTERFACE
class IMesh
{
public:
	virtual GeomType getGeomType()const = 0;
	virtual const unsigned* getIndices()const = 0;
	virtual bool hasIndices()const { return getIndices() != nullptr; }
	virtual const float* getAttribData(AttribLocation index)const = 0;
	virtual bool hasAttribData(AttribLocation index)const { return getAttribData(index) != nullptr; }
	virtual unsigned getNumVertices()const = 0;
	virtual unsigned getNumIndices()const = 0;
};

// COMMON STATIC TRIANGLE MESH
class Mesh : public IMesh
{	
public:
	Mesh()
	{
		numVertices = numTriangles = 0;
		positions = normals = tangents = colors = texCoords = nullptr;
		triangles = nullptr;
	}

	GeomType getGeomType()const { return GeomType::TRIANGLES; }
	const unsigned* getIndices()const { return triangles; }
	const float* getAttribData(AttribLocation index)const;
	unsigned getNumVertices()const { return numVertices; }
	unsigned getNumIndices()const { return 3 * numTriangles; }
	
	void initVertexData
	(
		unsigned numVertices,
		const float* positions,
		const float* normals,
		const float* tangents,
		const float* colors,
		const float* texCoords
	);

	void initTrianglesData
	(
		unsigned numTriangles,
		unsigned* triangles
	);

	// release RAM
	void free();

protected:
	unsigned numVertices;
	unsigned numTriangles;

	float* positions;
	float* normals;
	float* tangents;
	float* colors;
	float* texCoords;
	unsigned* triangles;
};