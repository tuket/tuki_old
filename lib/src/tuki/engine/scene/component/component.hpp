#pragma once

#include <tuki/util/id_map.hpp>
#include <memory>

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
	virtual std::unique_ptr<Component> create() = 0;
	virtual std::unique_ptr<Component> create(ComponentData* data) = 0;
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

	std::unique_ptr<Component> create(ComponentType type);
	std::unique_ptr<Component> create(const char* type);
	std::unique_ptr<Component> create(ComponentData* data);

	void addBuilder(ComponentBuilder* builder);
};

struct CompareComponentsByType {
	bool operator()(const Component* a, const Component* b) {
		return a->getComponentType() < b->getComponentType();
	}
};

// This Component is used to perform searches by type
class DummyComponent : public Component
{
	ComponentType type;
public:
	DummyComponent(ComponentType type) : type(type) {}
	SceneNode * getNode() { return nullptr; }
	virtual ComponentType getComponentType()const { return type; }
	virtual const char* getComponentTypeName()const { return "DummyComponent"; }
};