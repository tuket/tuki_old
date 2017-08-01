#ifndef MESH_HPP
#define MESH_HPP

struct Mesh
{
	
	unsigned numVertices;
	unsigned numTriangles;
	
	float* positions;
	float* normals;
	float* tangents;
	float* colors;
	float* texCoords;
	unsigned* triangles;
	
	Mesh()
	{
		numVertices = numTriangles = 0;
		positions = normals = tangents = colors = texCoords = nullptr;
		triangles = nullptr;
	}
	
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
	
};

enum class EAttribLocation
{
	POS,
	COLOR,
	TEX_COORD,
	NORMAL,
	TANGENT,
	BITANGENT,
	TEX_COORD_1,
	TEX_COORD_2,
	TEX_COORD_3,
};
enum class EAttribBitMask
{
	POS =			1 << (unsigned)EAttribLocation::POS,
	COLOR =			1 << (unsigned)EAttribLocation::COLOR,
	TEX_COORD =		1 << (unsigned)EAttribLocation::TEX_COORD,
	NORMAL =		1 << (unsigned)EAttribLocation::NORMAL,
	TANGENT =		1 << (unsigned)EAttribLocation::TANGENT,
	BITANGENT =		1 << (unsigned)EAttribLocation::BITANGENT,
	TEX_COORD_1 =	1 << (unsigned)EAttribLocation::TEX_COORD_1,
	TEX_COORD_2 =	1 << (unsigned)EAttribLocation::TEX_COORD_2,
	TEX_COORD_3 =	1 << (unsigned)EAttribLocation::TEX_COORD_3,
};
inline EAttribBitMask operator|(EAttribBitMask a, EAttribBitMask b)
{
	return static_cast<EAttribBitMask>( (int)a | (int)b );
}
inline EAttribBitMask operator&(EAttribBitMask a, EAttribBitMask b)
{
	return static_cast<EAttribBitMask>((int)a & (int)b);
}

struct VboSet
{
	int pos;
	int color;
	int texCoord;
	int normal;
	int tangent;
	// triangle indices
	int indices;
};

typedef int Vao;

struct MeshGpu
{
	Vao vao;
	VboSet vboSet;
	EAttribBitMask availableAttribs;

	// upload mesh from RAM to VRAM
	void load(const Mesh& mesh);

	// fre GPU memory
	void free();
};

// free GPU memory
void freeVao(Vao vao);
void freeVboSet(const VboSet& vbo);

#endif