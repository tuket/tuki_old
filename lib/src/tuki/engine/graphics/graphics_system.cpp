#include "graphics_system.hpp"

#include "../../render/mesh/mesh_pool.hpp"

using namespace std;

GraphicsSystem::GraphicsSystem()
{
	meshes = { new MeshChunk };
	materials = { new MaterialChunk };
	pointLights = { new PointLightChunk };

	// Sorry for the dirty trick but it is necessary for the following reason:
	// We want to make a linked list with the component slots that are free
	// In order to save memory we would like to make an union {component; nextFreePtr}
	// but C++ won't allow it because the components contain members with non-default constructors
	// Therefore we have to use all these ugly void* castings
	freeMeshSlot = &(*meshes[0])[0];
	for (int i = 0; i < MESHES_CHUNK_SIZE - 1; i++)
	{
		*(void**)&(*meshes[0])[i] = &(*meshes[0])[i + 1];
	}
	*(void**)&(*meshes[0])[MESHES_CHUNK_SIZE - 1] = nullptr;

	freeMaterialSlot = &(*materials[0])[0];
	for (int i = 0; i < MATERIALS_CHUNCK_SIZE - 1; i++)
	{
		*(void**)&(*materials[0])[i] = &(*materials[0])[i + 1];
	}
	*(void**)&materials[0][MATERIALS_CHUNCK_SIZE - 1] = nullptr;

	freePointLightSlot = &(*pointLights[0])[0];
	for (int i = 0; i < POINT_LIGHTS_CHUNK_SIZE - 1; i++) {
		*(void**)&(*pointLights[0])[i] = &(*pointLights[0])[i + 1];
	}
	*(void**)&(*pointLights[0])[POINT_LIGHTS_CHUNK_SIZE - 1] = nullptr;

}

MeshComponent* GraphicsSystem::createMeshComponent(const std::string& name)
{

}