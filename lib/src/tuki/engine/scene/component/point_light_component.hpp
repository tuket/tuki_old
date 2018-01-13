#pragma once

#include "component.hpp"
#include <glm/vec3.hpp>

class PointLightComponent : public IComponent
{
	friend class GraphicsSystem;
public:
	glm::vec3 intensity;
	ComponentType getComponentType()const { return ComponentType::PointLightComponent; }
	static ComponentType getComponentTypeStatic() { return ComponentType::PointLightComponent; }
};