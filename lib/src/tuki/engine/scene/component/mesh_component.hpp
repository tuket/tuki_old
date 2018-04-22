#pragma once

#include "component.hpp"

#include "../../../render/mesh/mesh_pool.hpp"
#include "tuki/util/mallocr/mallocr_chunk.hpp"

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
	handle<Component> create()override;
	handle<Component> create(ComponentData* data)override { create((MeshComponentData*)data); }
	handle<Component> create(MeshComponentData* data);
	void destroy(handle<Component> component)override;
	ComponentType getComponentType()const override{ return ComponentTypes::Mesh; }
private:
	Mallocr_Chunk<MeshComponent> mallocr;
};