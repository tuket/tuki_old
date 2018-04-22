#pragma once

#include "component.hpp"
#include <glm/vec3.hpp>

class PointLightComponent : public Component
{
	friend class GraphicsSystem;
public:
	glm::vec3 intensity;
	ComponentType getComponentType()const { return ComponentTypes::PointLight; }
	static ComponentType getComponentTypeStatic() { return ComponentTypes::PointLight; }
};