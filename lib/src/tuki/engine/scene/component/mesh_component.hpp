#pragma once

#include "component.hpp"

#include "../../../render/mesh/mesh_pool.hpp"

class MeshComponent : public IComponent
{
	friend class GraphicsSystem;
	MeshGpuHandle meshGpu;
public:
	ComponentType getComponentType()const { return ComponentType::MeshComponent; }
	static ComponentType getComponentTypeStatic() { return ComponentType::MeshComponent; }
};