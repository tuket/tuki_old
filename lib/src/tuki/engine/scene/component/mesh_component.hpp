#pragma once

#include "component.hpp"

class IMeshGpu;

class MeshComponent : public IComponent
{
	friend class GraphicsSystem;
	IMeshGpu* meshGpu;
public:
	ComponentType getComponentType()const { return ComponentType::MeshComponent; }
	static ComponentType getComponentTypeStatic() { return ComponentType::MeshComponent; }
};