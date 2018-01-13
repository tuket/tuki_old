#pragma once

#include "component.hpp"
#include "../../../render/material/material.hpp"

class MaterialComponent : public IComponent
{
	friend class GraphicsSystem;
public:
	Material material;
	ComponentType getComponentType()const { return ComponentType::MaterialComponent; }
	static ComponentType getComponentTypeStatic() { return ComponentType::MaterialComponent; }
};