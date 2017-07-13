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
		positions = normals = tangents = colors = texCoords = triangles = nullptr;
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
	
};

struct VertexAttribLocations
{
	int pos;
	int color;
	int texCoord;
	int normal;
	int tangent;
};

enum EMeshAttribLocation
{
	pos = 0,
	color,
	texCoord,
	normal,
	tangent
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

	// upload mesh from RAM to VRAM
	void load(const Mesh& mesh);
	// fre GPU memory
	void free();
}

// free main memory
static inline void freeMesh(Mesh& mesh)
{
	delete[] mesh.positions;
	delete[] mesh.normals;
	delete[] mesh.tangents;
	delete[] mesh.colors;
	delete[] mesh.texCoords;
	delete[] mesh.triangles;
};

// free GPU memory
void freeVao(Vao& vao);
void freeVboSet(VboSet& vbo);

#endif