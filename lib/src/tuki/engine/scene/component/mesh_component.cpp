#include "mesh_component.hpp"

handle<Component> MeshComponentBuilder::create()
{
	return mallocr.create();
}

handle<Component> MeshComponentBuilder::create(MeshComponentData* data)
{

}

void MeshComponentBuilder::destroy(handle<Component> component)
{

}