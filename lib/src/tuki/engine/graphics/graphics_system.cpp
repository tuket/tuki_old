#include "graphics_system.hpp"

#include "../../render/mesh/mesh_pool.hpp"

using namespace std;
using namespace glm;

GraphicsSystem::GraphicsSystem()
{
	meshes = { new MeshChunk };
	materials = { new MaterialChunk };
	pointLights = { new PointLightChunk };

	initMeshChunk(0);
	initMaterialChunk(0);
	initPointLightChunk(0);
}

MeshComponent* GraphicsSystem::createMeshComponent(const std::string& name)
{
	if (freeMeshSlot == nullptr) addMeshChunk();

	MeshComponent* c = freeMeshSlot;
	freeMeshSlot = *(MeshComponent**)c;
	c->node = nullptr;
	c->meshGpu = MeshPool::getSingleton()->getMesh(name);
	return c;
}

MaterialComponent* GraphicsSystem::createMaterialComponent(const std::string& name)
{
	if (freeMaterialSlot == nullptr) addMaterialChunk();

	MaterialComponent* c = freeMaterialSlot;
	c->node = nullptr;
	c->material = MaterialManager::getSingleton()->loadMaterial(name);
	return c;
}

PointLightComponent* GraphicsSystem::createPointLightComponent()
{
	if (freePointLightSlot == nullptr) addPointLightChunk();

	PointLightComponent* c = freePointLightSlot;
	c->node = nullptr;
	c->intensity = vec3(1.f);
	return c;
}

void GraphicsSystem::releaseComponent(MeshComponent* c)
{
	c->node = nullptr;
	c->meshGpu = MeshGpuHandle::NONE;

	MeshComponent** ptr = (MeshComponent**)c;
	MeshComponent* prevFree = freeMeshSlot;
	freeMeshSlot = c;
	*ptr = prevFree;
}

void GraphicsSystem::releaseComponent(MaterialComponent* c)
{
	c->node = nullptr;
	c->material.release();

	MaterialComponent** ptr = (MaterialComponent**)c;
	MaterialComponent* prevFree = freeMaterialSlot;
	freeMaterialSlot = c;
	*ptr = prevFree;
}

void GraphicsSystem::releaseComponent(PointLightComponent* c)
{
	c->node = nullptr;
	c->intensity = vec3(0);

	PointLightComponent** ptr = (PointLightComponent**)c;
	PointLightComponent* prevFree = freePointLightSlot;
	freePointLightSlot = c;
	*ptr = prevFree;
}

// Sorry for the dirty trick but it is necessary for the following reason:
// We want to make a linked list with the component slots that are free
// In order to save memory we would like to make an union {component; nextFreePtr}
// but C++ won't allow it because the components contain members with non-default constructors
// Therefore we have to use all these ugly void* castings
void GraphicsSystem::initMeshChunk(unsigned i)
{
	freeMeshSlot = accessMeshChunkSlot(i, 0);
	for (int j = 0; j < MESHES_CHUNK_SIZE - 1; j++)
	{
		*(void**)accessMeshChunkSlot(i, j) = accessMeshChunkSlot(i, j+1);
	}
	*(void**)accessMeshChunkSlot(i, MESHES_CHUNK_SIZE-1) = nullptr;
}

void GraphicsSystem::initMaterialChunk(unsigned i)
{
	freeMaterialSlot = accessMaterialChunkSlot(i, 0);
	for (int j = 0; j < MATERIALS_CHUNCK_SIZE - 1; j++)
	{
		*(void**)accessMaterialChunkSlot(i, j) = accessMaterialChunkSlot(i, j+1);
	}
	*(void**)accessMaterialChunkSlot(i, MATERIALS_CHUNCK_SIZE-1) = nullptr;
}

void GraphicsSystem::initPointLightChunk(unsigned i)
{
	freePointLightSlot = accessPointLightChunkSlot(i, 0);
	for (int j = 0; j < POINT_LIGHTS_CHUNK_SIZE - 1; j++) {
		*(void**)accessPointLightChunkSlot(i, j) = accessPointLightChunkSlot(i, j+1);
	}
	*(void**)accessPointLightChunkSlot(i, POINT_LIGHTS_CHUNK_SIZE-1) = nullptr;
}

void GraphicsSystem::addMeshChunk()
{
	assert(freeMeshSlot == nullptr);
	unsigned i = meshes.size();
	meshes.push_back(new MeshChunk);
	initMeshChunk(i);
}

void GraphicsSystem::addMaterialChunk()
{
	assert(freeMaterialSlot == nullptr);
	unsigned i = materials.size();
	materials.push_back(new MaterialChunk);
	initMaterialChunk(i);
}

void GraphicsSystem::addPointLightChunk()
{
	assert(freePointLightSlot == nullptr);
	unsigned i = pointLights.size();
	pointLights.push_back(new PointLightChunk);
	initPointLightChunk(i);
}