#include "scene_node.hpp"

#include "scene.hpp"

using namespace std;
using namespace glm;

IComponent* SceneNode::getComponent(ComponentType type)const
{
	for (IComponent* comp : components)
	{
		if (comp->getComponentType() == type)
			return comp;
	}
	return nullptr;
}

vector<IComponent*> SceneNode::getComponents(ComponentType type) const
{
	vector<IComponent*> res;
	int n = 0;
	for (IComponent* comp : components)
	{
		if (comp->getComponentType() == type) n++;
	}
	res.reserve(n);
	for (IComponent* comp : components)
	{
		if (comp->getComponentType() == type) res.push_back(comp);
	}
	return res;
}

void SceneNode::attachComponent(IComponent* component)
{
	components.push_back(component);
}

void SceneNode::attachChild(SceneNode* child)
{
	if (nameToChild.find(child->name) != nameToChild.end())
		throw runtime_error("can't attach child: there is a child with that name already");

	children.push_back(child);
	child->parent = this;
	nameToChild[child->name] = child;
}

void SceneNode::unattachChild(SceneNode* child)
{
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		if (*it == child)
		{
			children.erase(it);
			child->parent = nullptr;
			return;
		}
	}
	throw runtime_error("can't unattach child: it is not among children");
}

std::string SceneNode::getName()const
{
	return name;
}

void SceneNode::setName(const std::string& newName)
{
	if (newName == name) return;
	auto it = parent->nameToChild.find(newName);
	if (it != parent->nameToChild.end())
		throw runtime_error("can't change child name: there is a child with that name already");

	parent->nameToChild.erase(it);
	parent->nameToChild[newName] = this;
	name = newName;
}

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

void SceneNode::destroy()
{
	if (parent)
	{
		auto it = parent->children.begin();
		while (it != parent->children.end()) ++it;
		assert(it == parent->children.end() && "parent doesn't have this child");
		parent->children.erase(it);
	}
	delete this;
}

// PRIVATE
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