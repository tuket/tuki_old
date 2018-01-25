#pragma once

#include "../gl/mesh_gpu.hpp"
#include <vector>
#include <map>
#include <string>
#include "../../util/singleton.hpp"

typedef int MeshId;

class MeshGpuHandle
{
	friend class MeshPool;

	MeshId id;
public:
	MeshGpuHandle() : id(-1) {}
	MeshGpuHandle(const MeshGpuHandle& handle);
	MeshGpuHandle operator=(const MeshGpuHandle& handle);

	const MeshGpu* operator->();

	~MeshGpuHandle();

	static MeshGpuHandle NONE;
};

class MeshPool : public Singleton<MeshPool>
{
	friend class Singleton<MeshPool>;
	friend class MeshGpuHandle;
	MeshPool();
public:
	MeshGpuHandle getMesh(const std::string& name);
	bool isMeshInGpu(const std::string& name);
	void dec(MeshId id);
	void inc(MeshId id);

private:
	int nextFreeId;
	std::vector<MeshGpu> vmesh;
	std::vector<int> refcount;	// ref cout if negative, next free if positive
	std::map<std::string, MeshId> nameToId;
	std::vector<std::string> idToName;

	void release(MeshId id);
	MeshId load(const std::string& fileName);
	void expand();
	void expandIfNeeded();
};