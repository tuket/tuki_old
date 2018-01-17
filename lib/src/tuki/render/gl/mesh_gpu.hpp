#pragma once

#include "attribs.hpp"
#include "attrib_initializers.hpp"
#include "../mesh/mesh.hpp"

typedef int Vao;
typedef int Vbo;

struct VboSet
{
	Vbo attribs[(int)AttribLocation::NUM_ATTRIBS];
	// triangle indices
	Vbo indices;
};

// MESH IN THE GPU

// abstract class for the many types of possible meshes in gpu
class MeshGpu
{
public:
	bool hasIndices()const { return vboSet.indices >= 0; }
	GeomType getGeomType()const { return geomType; }
	AttribBitMask getAttribBitMask()const { return attribBitMask; }
	AttribInitilizer getAttribInitializer()const { return AttribInitilizers::generic; }
	unsigned getNumElements()const { return numElements; }

	void bind()const;

	// fre GPU memory
	void free();

	static void initScreenQuad(MeshGpu& out);
	static void initFromMesh(MeshGpu& out, const IMesh& mesh);

protected:
	Vao vao;
	VboSet vboSet;
	AttribBitMask attribBitMask;
	GeomType geomType = GeomType::TRIANGLES;
	unsigned numElements;
};

// free GPU memory
void freeVao(Vao vao);
void freeVbo(Vbo vbo);
void freeVaos(const Vao* vaos, unsigned num);
void freeVbos(const Vbo* vbos, unsigned num);
void freeVboSet(const VboSet& vboSet);