#pragma once

#include "component.hpp"

#include "../../../render/mesh/mesh_pool.hpp"

class MeshComponent : public Component
{
	friend class GraphicsSystem;
	friend class MeshComponentBuilder;

public:
	ComponentType getComponentType()const { return ComponentTypes::Mesh; }
	static ComponentType getComponentTypeStatic() { return ComponentTypes::Mesh; }

private:
	MeshGpuHandle meshGpu;
};

class MeshComponentData : public ComponentData
{
	ComponentType getComponentType()const override { return ComponentTypes::Mesh; }
private:
	std::string fileName;
};

class MeshComponentBuilder : public ComponentBuilder
{
public:
	Component* create()override;
	Component* create(ComponentData* data)override { create((MeshComponentData*)data); }
	Component* create(MeshComponentData* data);
	ComponentType getComponentType()const override{ return ComponentTypes::Mesh; }
};