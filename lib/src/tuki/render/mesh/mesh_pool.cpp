#include "mesh_pool.hpp"

const unsigned INITIAL_MESH_VECTOR_SIZE = 128;

MeshGpuHandle::MeshGpuHandle(const MeshGpuHandle& handle)
{
	id = handle.id;
	getMeshPool().inc(handle.id);
}

MeshGpuHandle::~MeshGpuHandle()
{
	getMeshPool().dec(id);
}

MeshPool& MeshGpuHandle::getMeshPool()const
{
	// TODO
}

// MeshPool

MeshPool::MeshPool()
	: refcount(INITIAL_MESH_VECTOR_SIZE)
{
	nextFreeId = -1;	// -1 means index 0, -2 index 1, and so on
	for (int i = 0; i < INITIAL_MESH_VECTOR_SIZE; i++)
	{
		refcount[i] = i + 1;
	}
}

bool MeshPool::isMeshInGpu(const std::string& name)
{
	MeshGpuHandle handle;
	auto it = nameToId.find(name);
	return it != nameToId.end();
}

MeshGpuHandle MeshPool::getMesh(const std::string& name)
{
	MeshGpuHandle handle;
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		// the mesh is already allocated
		MeshId id = it->second;
		handle.id = id;
		inc(id);
	}
	else
	{
		// the mesh must be allocated
		handle.id = load(name);
		refcount[handle.id] = -1;
	}
	return handle;
}

void MeshPool::dec(MeshId id)
{
	refcount[id]++;
	if (refcount[id] == 0) release(id);
}

void MeshPool::inc(MeshId id)
{
	refcount[id]--;
}


void MeshPool::release(MeshId id)
{
	vmesh[id].free();
	refcount[id] = nextFreeId;
	nextFreeId = id;
}

MeshId MeshPool::load(const std::string& fileName)
{
	Mesh mesh;
	mesh.load(fileName);
	expandIfNeeded();
	MeshGpu::initFromMesh(vmesh[nextFreeId], mesh);
	MeshGpuHandle handle;
	handle.id = nextFreeId;
	nextFreeId = refcount[nextFreeId];
	refcount[handle.id] = -1;
}

void MeshPool::expand()
{
	const unsigned prevSize = vmesh.size();
	const unsigned newSize = prevSize + INITIAL_MESH_VECTOR_SIZE;
	vmesh.resize(newSize);
	refcount.resize(newSize);
	idToName.resize(newSize);
	for (int i = prevSize; i < newSize; i++)
	{
		refcount[i] = i + 1;
	}
}

void MeshPool::expandIfNeeded()
{
	if (nextFreeId < 0) expand();
}