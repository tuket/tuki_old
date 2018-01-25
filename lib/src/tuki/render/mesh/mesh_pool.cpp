#include "mesh_pool.hpp"

#include <exception>

using namespace std;

const unsigned INITIAL_MESH_VECTOR_SIZE = 128;

MeshGpuHandle::MeshGpuHandle(const MeshGpuHandle& handle)
{
	id = handle.id;
	MeshPool::getSingleton()->inc(handle.id);
}

MeshGpuHandle MeshGpuHandle::operator=(const MeshGpuHandle& handle)
{
	MeshId prevId = id;
	id = handle.id;
	MeshPool::getSingleton()->inc(handle.id);
	if (prevId != -1) {
		MeshPool::getSingleton()->dec(prevId);
	}
	return *this;
}

const MeshGpu* MeshGpuHandle::operator->()
{
	MeshPool* meshPool = MeshPool::getSingleton();
	if (meshPool->refcount[id] >= 0) throw runtime_error("this mesh has been released");
	return &meshPool->vmesh[id];
}

MeshGpuHandle::~MeshGpuHandle()
{
	MeshPool::getSingleton()->dec(id);
}

// MeshPool

MeshPool::MeshPool()
	: refcount(INITIAL_MESH_VECTOR_SIZE)
	, vmesh(INITIAL_MESH_VECTOR_SIZE)
{
	nextFreeId = 0;
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
	mesh.initFromFile(fileName);
	expandIfNeeded();
	MeshGpu::initFromMesh(vmesh[nextFreeId], mesh);
	MeshId id = nextFreeId;
	nextFreeId = refcount[nextFreeId];
	refcount[id] = -1;
	return id;
}

void MeshPool::expand()
{
	const unsigned prevSize = vmesh.size();
	const unsigned newSize = prevSize + INITIAL_MESH_VECTOR_SIZE;
	vmesh.resize(newSize);
	refcount.resize(newSize);
	idToName.resize(newSize);
	for (unsigned i = prevSize; i < newSize; i++)
	{
		refcount[i] = i + 1;
	}
}

void MeshPool::expandIfNeeded()
{
	if (nextFreeId < 0) expand();
}