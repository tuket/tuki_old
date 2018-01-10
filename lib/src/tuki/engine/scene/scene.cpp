#include "scene.hpp"

void Scene::addDirty(const SceneNode* node)
{
	dirtyNodes.insert(node);
}

void Scene::removeDirty(const SceneNode* node)
{
	dirtyNodes.erase(node);
}