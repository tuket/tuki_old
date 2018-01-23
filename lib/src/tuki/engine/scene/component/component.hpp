#pragma once

class SceneNode;

enum class ComponentType
{
	MeshComponent,
	MaterialComponent,
	PointLightComponent,

	COUNT,
};

class IComponent
{
public:
	SceneNode* getNode() { return node; }
	ComponentType getComponentType()const { return ComponentType::COUNT; }

protected:
	SceneNode* node;
};