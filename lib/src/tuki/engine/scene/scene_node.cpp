#include "scene_node.hpp"

#include "scene.hpp"

using namespace std;
using namespace glm;

const mat4& SceneNode::getTransformMatrix()const
{
	if (dirtyFlag)
	{
		recomputeTransMat();
		scene->removeDirty(this);
		for (SceneNode* node : children) scene->addDirty(node);
	}
	return transMat;
}

const mat4& SceneNode::getGlobalTransformMatrix()const
{
	recomputeGlobalTransMat();
	return globalTransMat;
}
const vec3 SceneNode::getGlobalPosition()const
{
	recomputeGlobalTransMat();
	vec3 res(globalTransMat[3][0], globalTransMat[3][1], globalTransMat[3][2]);
	return res;
}
const quat SceneNode::getGlobalRotation()const
{
	recomputeGlobalTransMat();
	return quat_cast(globalTransMat);
}

bool SceneNode::hasDirtyParent()const
{
	bool res = false;
	SceneNode* node = parent;
	while (node)
	{
		if (node->isDirty())
		{
			res = true;
			break;
		}
		node = node->parent;
	}
	return res;
}

void SceneNode::setDirty()
{
	scene->addDirty(this);
	dirtyFlag = true;
}

void SceneNode::recomputeTransMat()const
{
	transMat = mat4_cast(trans.rot);
	transMat[3][0] = trans.pos.x;
	transMat[3][1] = trans.pos.y;
	transMat[3][2] = trans.pos.z;
}

void SceneNode::recomputeGlobalTransMat()const
{
	if (hasDirtyParent())
	{
		vector<SceneNode*> parents;
		parents.reserve(32);
		SceneNode* node = parent;
		while (node)
		{
			parents.push_back(node);
			node = node->parent;
		}
		int N = parents.size();
		int i;
		// goto to the top dirty parent
		for (i = N - 1; ; i--) if (parents[i]->isDirty()) break;
		// if the root is dirty recompute transf matrix
		if (!parents[i]->parent)
		{
			node->recomputeTransMat();
			node->globalTransMat = node->transMat;
			i--;
		}
		// recursivelly recompute transform matrices
		for (; i >= 0; i--)
		{
			node = parents[i];
			// if dirty recompute local
			if (node->isDirty())
			{
				node->recomputeTransMat();
			}
			// recompute global
			node->globalTransMat = parents[i + 1]->globalTransMat * node->transMat;
		}
	}

	if (dirtyFlag)
	{
		recomputeTransMat();
	}

	// recompute *this globalTransMat
	globalTransMat = parent->globalTransMat * transMat;
}