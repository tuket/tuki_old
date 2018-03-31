#pragma once

#include "util/mallocr/mallocr_simple.hpp"
#include <type_traits>

class SceneNode;

template <typename Comp>
class ComponentFactory : public Mallocr_Simple
{
	static_assert(std::is_base_of<Component, Comp>::value);
};

class Component
{
public:
	typedef std::string Type;

	Component() : node(nullptr) {}
	// Get the component it's attachde to
	SceneNode* getNode() { return node; }
	// Get the type of component
	virtual Type getType()const = 0;

protected:
	SceneNode* node;
};