#ifndef MESH_HPP
#define MESH_HPP

enum class EGeomType
{
	POINTS,
	LINES,
	TRIANGLES,
	LINES_STRIP,
	TRIANGLE_STRIP,
	TRIANGLE_FAN
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

	NUM_ATTRIBS
};
enum class EAttribBitMask
{
	NONE = 0,
	POS = 1 << (unsigned)EAttribLocation::POS,
	COLOR = 1 << (unsigned)EAttribLocation::COLOR,
	TEX_COORD = 1 << (unsigned)EAttribLocation::TEX_COORD,
	NORMAL = 1 << (unsigned)EAttribLocation::NORMAL,
	TANGENT = 1 << (unsigned)EAttribLocation::TANGENT,
	BITANGENT = 1 << (unsigned)EAttribLocation::BITANGENT,
	TEX_COORD_1 = 1 << (unsigned)EAttribLocation::TEX_COORD_1,
	TEX_COORD_2 = 1 << (unsigned)EAttribLocation::TEX_COORD_2,
	TEX_COORD_3 = 1 << (unsigned)EAttribLocation::TEX_COORD_3,
};
inline EAttribBitMask operator|(EAttribBitMask a, EAttribBitMask b)
{
	return static_cast<EAttribBitMask>((int)a | (int)b);
}
inline EAttribBitMask operator&(EAttribBitMask a, EAttribBitMask b)
{
	return static_cast<EAttribBitMask>((int)a & (int)b);
}
inline EAttribBitMask& operator |=(EAttribBitMask& a, EAttribBitMask b)
{
	return a = a | b;
}
inline EAttribBitMask& operator &=(EAttribBitMask& a, EAttribBitMask b)
{
	return a = a & b;
}

// MESH INTERFACE
struct IMesh
{
public:
	virtual EGeomType getGeomType()const = 0;
	virtual const unsigned* getIndices()const = 0;
	virtual bool hasIndices()const { return getIndices() != nullptr; }
	virtual const float* getAttribData(EAttribLocation index)const = 0;
	virtual bool hasAttribData(EAttribLocation index)const { return getAttribData(index) != nullptr; }
	virtual unsigned getNumVertices()const = 0;
	virtual unsigned getNumIndices()const = 0;
};

// COMMON STATIC MESH
struct Mesh : IMesh
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

	EGeomType getGeomType()const { return EGeomType::TRIANGLES; }
	const unsigned* getIndices() { return triangles; }
	const float* getAttribData(EAttribLocation index)const;
	unsigned getNumVertices()const { return numVertices; }
	unsigned getNunIndices()const { return numTriangles; }
	
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

struct VboSet
{
	int attribs[(int)EAttribLocation::NUM_ATTRIBS];
	// triangle indices
	int indices;
};

typedef int Vao;

// MESH IN THE GPU

// abstract class for the many types of possible meshes in gpu
struct IMeshGpu
{
	Vao vao;
	EAttribBitMask attribBitMask;

	// upload mesh from RAM to VRAM
	void load(const IMesh& mesh) = 0;

	// fre GPU memory
	virtual void free() = 0;
};

// can be used for any type of mesh configuration
// for better performance inherit from IMesh and make more specific implementation
struct MeshGpuGeneric : IMeshGpu
{
	VboSet vboSet;
};

struct IMeshGpu
{
	Vao vao;
	VboSet vboSet;
	EAttribBitMask availableAttribs;

	// upload mesh from RAM to VRAM
	void load(const IMesh& mesh);
	void load(const Mesh& mesh);

	// fre GPU memory
	void free();
};

// free GPU memory
void freeVao(Vao vao);
void freeVboSet(const VboSet& vbo);

#endif