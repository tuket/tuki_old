#pragma once

class SceneNode;

class IComponent
{
public:
	SceneNode* getNode() { return node; }

private:
	SceneNode* node;
};