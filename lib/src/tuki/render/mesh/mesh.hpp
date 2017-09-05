#pragma once

#include "attrib_initializers.hpp"

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

enum class AttribLocation
{
	POS = 0,	// must be 0!
	COLOR,
	TEX_COORD,
	NORMAL,
	TANGENT,
	BITANGENT,
	TEX_COORD_1,
	TEX_COORD_2,
	TEX_COORD_3,

	NUM_ATTRIBS
};
enum class AttribBitMask
{
	NONE = 0,
	POS = 1 << (unsigned)AttribLocation::POS,
	COLOR = 1 << (unsigned)AttribLocation::COLOR,
	TEX_COORD = 1 << (unsigned)AttribLocation::TEX_COORD,
	NORMAL = 1 << (unsigned)AttribLocation::NORMAL,
	TANGENT = 1 << (unsigned)AttribLocation::TANGENT,
	BITANGENT = 1 << (unsigned)AttribLocation::BITANGENT,
	TEX_COORD_1 = 1 << (unsigned)AttribLocation::TEX_COORD_1,
	TEX_COORD_2 = 1 << (unsigned)AttribLocation::TEX_COORD_2,
	TEX_COORD_3 = 1 << (unsigned)AttribLocation::TEX_COORD_3,
};
inline AttribBitMask operator|(AttribBitMask a, AttribBitMask b)
{
	return static_cast<AttribBitMask>((int)a | (int)b);
}
inline AttribBitMask operator&(AttribBitMask a, AttribBitMask b)
{
	return static_cast<AttribBitMask>((int)a & (int)b);
}
inline AttribBitMask& operator |=(AttribBitMask& a, AttribBitMask b)
{
	return a = a | b;
}
inline AttribBitMask& operator &=(AttribBitMask& a, AttribBitMask b)
{
	return a = a & b;
}

const unsigned ATTRIB_NUM_COMPONENTS[(int)AttribLocation::NUM_ATTRIBS] =
{
	3,		// POS
	3,		// COLOR
	2,		// TEX_COORD
	3,		// NORMAL
	3,		// TANGENT
	3,		// BITANGENT
	2,		// TEX_COORD_1
	2,		// TEX_COORD_2
	2,		// TEX_COORD_3
};

extern const char* ATTRIB_NAMES[];

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

typedef int Vao;
typedef int Vbo;

struct VboSetFull
{
	Vbo attribs[(int)AttribLocation::NUM_ATTRIBS];
	// triangle indices
	Vbo indices;
};

// MESH IN THE GPU

// abstract class for the many types of possible meshes in gpu
class IMeshGpu
{
public:
	virtual bool hasIndices()const = 0;
	virtual GeomType getGeomType()const = 0;
	virtual AttribBitMask getAttribBitMask()const = 0;
	virtual AttribInitilizer getAttribInitializer()const { return AttribInitilizers::generic; }

	virtual unsigned getNumElements()const = 0;

	void bind()const;

	// upload mesh from RAM to VRAM
	virtual void load(const IMesh& mesh) {};

	// fre GPU memory
	virtual void free() = 0;

protected:
	Vao vao;
};

// can be used for any type of triangle mesh configuration
// for better performance inherit from IMesh and make a more specific implementation
class MeshGpuGeneric : public IMeshGpu
{
public:
	bool hasIndices()const { return vboSet.indices >= 0; }
	GeomType getGeomType()const { return GeomType::TRIANGLES; }
	AttribBitMask getAttribBitMask()const { return attribBitMask; }

	unsigned getNumElements()const;

	void load(const IMesh& mesh);
	void free();

protected:
	VboSetFull vboSet;
	AttribBitMask attribBitMask;
	unsigned numElements;
};

class UvPlaneMeshGpu : public IMeshGpu
{
public:
	bool hasIndices()const { return false; }
	GeomType getGeomType()const { return GeomType::TRIANGLE_STRIP; }
	AttribBitMask getAttribBitMask()const { return AttribBitMask::TEX_COORD; }

	unsigned getNumElements()const { return 4; }

	void load();
	void free();

protected:
	Vbo vbo;
};

// free GPU memory
void freeVao(Vao vao);
void freeVbo(Vbo vbo);
void freeVaos(const Vao* vaos, unsigned num);
void freeVbos(const Vbo* vbos, unsigned num);
void freeVboSet(const VboSetFull& vboSet);