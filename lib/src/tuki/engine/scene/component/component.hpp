#pragma once

class SceneNode;

enum class ComponentType
{
	MeshComponent,
	MaterialComponent,
	PointLightComponent
};

class IComponent
{
public:
	SceneNode* getNode() { return node; }
	virtual ComponentType getComponentType()const = 0;

private:
	SceneNode* node;
};