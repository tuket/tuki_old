#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <map>
#include "component/component.hpp"

class IMeshGpu;
class Scene;

struct Transform
{
	glm::vec3 pos;
	glm::quat rot;
};

class SceneNode
{
public:

	SceneNode* getParent()const { return parent; }
	const std::vector<SceneNode*>& getChidren()const { return children; }
	Scene* getScene()const { return scene; }

	IComponent* getComponent(ComponentType type)const;
	template <class T>
	T* getComponent() const
	{
		return static_cast<T*>(getComponent(T::getComponentTypeStatic()));
	}

	std::vector<IComponent*> getComponents(ComponentType type) const;
	template <class T>
	std::vector<T*> getComponents() const
	{
		return static_cast<std::vector<T*>>(getComponent(T::getComponentTypeStatic()));
	}

	void attachComponent(IComponent* component);
	void attachChild(SceneNode* child);
	void unattachChild(SceneNode* child);

	std::string getName()const;
	void setName(const std::string& newName);

	const Transform& getTransform()const { return trans; }
	void setTransform(const Transform& trans) { this->trans = trans; setDirty(); }
	const glm::vec3& getPosition()const { return trans.pos; }
	const glm::quat& getRotation()const { return trans.rot; }
	void setPosition(const glm::vec3& pos) { trans.pos = pos; setDirty(); }
	void setRotation(const glm::quat& rot) { trans.rot = rot; setDirty(); }

	const glm::mat4& getTransformMatrix()const;

	const glm::mat4& getGlobalTransformMatrix()const;
	const glm::vec3 getGlobalPosition()const;
	const glm::quat getGlobalRotation()const;

	void destroy();

private:

	SceneNode() : parent(nullptr), id(-1), scene(nullptr) { components.reserve(4); }
	int id;
	std::string name;
	Transform trans;
	mutable glm::mat4 transMat;
	mutable glm::mat4 globalTransMat;
	mutable bool dirtyFlag;		// if true, 
	Scene* scene;
	SceneNode* parent;
	std::vector<SceneNode*> children;
	std::map<std::string, SceneNode*> nameToChild;
	std::vector<IComponent*> components;

	// COMPONENTS
	IMeshGpu* meshGpu;

	// set the dirty flag to true, which means that the transform matrix must be recomputed recursivelly
	void setDirty();
	bool isDirty()const { return dirtyFlag; }
	bool hasDirtyParent()const;
	void recomputeTransMat()const;
	void recomputeGlobalTransMat()const;
};