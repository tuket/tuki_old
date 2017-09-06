#pragma once

#include "attribs.hpp"
#include "attrib_initializers.hpp"
#include "../mesh/mesh.hpp"

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