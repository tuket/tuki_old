#pragma once

#include <tuki/util/id_map.hpp>

class SceneNode;
typedef int ComponentType;

namespace ComponentTypes
{
	enum {
		Mesh,
		Material,
		PointLight,

		COUNT
	};
};

class Component
{
public:
	SceneNode* getNode() { return node; }
	virtual ComponentType getComponentType()const = 0;
	virtual const char* getComponentTypeName()const = 0;
protected:
	SceneNode* node;
};

// you can use this data to build a component
struct ComponentData
{
	virtual ComponentType getComponentType()const = 0;
};

struct ComponentBuilder
{
	virtual Component* create() = 0;
	virtual Component* create(ComponentData* data) = 0;
	virtual ComponentType getComponentType()const = 0;
};

class ComponentFactory
{
	IdMap idMap;
	std::vector<ComponentBuilder*> builders;
public:
	ComponentFactory() {}

	template <typename C>
	void registerType() {
		std::string name = C::getComponentTypeNameStatic();
		idMap.add(name);
	}

	Component* create(ComponentType type);
	Component* create(const char* type);
	Component* create(ComponentData* data);

	void addBuilder(ComponentBuilder* builder);
};