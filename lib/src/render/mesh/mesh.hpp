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

const unsigned ATTRIB_NUM_COMPONENTS[(int)EAttribLocation::NUM_ATTRIBS] =
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

// COMMON STATIC TRIANGLE MESH
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
	const unsigned* getIndices()const { return triangles; }
	const float* getAttribData(EAttribLocation index)const;
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
	
};

typedef int Vao;
typedef int Vbo;

struct VboSetFull
{
	Vbo attribs[(int)EAttribLocation::NUM_ATTRIBS];
	// triangle indices
	Vbo indices;
};

// MESH IN THE GPU

// abstract class for the many types of possible meshes in gpu
struct IMeshGpu
{
	Vao vao;

	virtual bool hasIndices()const = 0;
	virtual EGeomType getGeomType()const = 0;
	virtual EAttribBitMask getAttribBitMask()const = 0;

	void bind()const;

	// upload mesh from RAM to VRAM
	virtual void load(const IMesh& mesh) {};

	// fre GPU memory
	virtual void free() = 0;
};

// can be used for any type of triangle mesh configuration
// for better performance inherit from IMesh and make a more specific implementation
struct MeshGpuGeneric : IMeshGpu
{
	VboSetFull vboSet;
	EAttribBitMask attribBitMask;

	bool hasIndices()const { return vboSet.indices >= 0; }
	EGeomType getGeomType()const { return EGeomType::TRIANGLES; }
	EAttribBitMask getAttribBitMask()const { return attribBitMask; }

	void load(const IMesh& mesh);
	void load(const Mesh& mesh);
	void free();
};

struct UvPlaneMeshGpu : IMeshGpu
{
	Vbo vbo;

	bool hasIndices()const { return false; }
	EGeomType getGeomType()const { return EGeomType::TRIANGLE_STRIP; }
	EAttribBitMask getAttribBitMask()const { return EAttribBitMask::TEX_COORD; }

	void load();
	void free();
};

// free GPU memory
void freeVao(Vao vao);
void freeVbo(Vbo vbo);
void freeVaos(const Vao* vaos, unsigned num);
void freeVbos(const Vbo* vbos, unsigned num);

#endif