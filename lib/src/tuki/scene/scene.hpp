#pragma once

#include <set>

class SceneNode;

class Scene
{
	friend class SceneNode;

public:

	SceneNode* getRoot()const { return root; }

private:
	SceneNode* root;
	std::set<SceneNode*> dirtyNodes;	// nodes that have dirty flag set to true

	void addDirty(const SceneNode* node);
	void removeDirty(const SceneNode* node);
};