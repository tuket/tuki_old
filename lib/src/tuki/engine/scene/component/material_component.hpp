#pragma once

#include "component.hpp"
#include "../../../render/material/material.hpp"

class MaterialComponent : public Component
{
	friend class GraphicsSystem;
public:
	Material material;
	ComponentType getComponentType()const { return ComponentType::Material; }
	static ComponentType getComponentTypeStatic() { return ComponentType::Material; }
};