#include "component.hpp"
#include <cassert>
#include <stdexcept>

using namespace std;

Component* ComponentFactory::create(ComponentType type) {
	if (type >= builders.size())
		throw invalid_argument("type >= builders.size()");
	ComponentBuilder* builder = builders[type];
	if (!builder)
		throw invalid_argument("builder is null");
	return builder->create();
}

Component* ComponentFactory::create(const char* typeName) {
	ComponentType type = idMap.id(typeName);
	return create(type);
}

Component* ComponentFactory::create(ComponentData* data)
{
	ComponentType type = data->getComponentType();
	if (builders.size() >= type || builders[type] == nullptr)
		throw invalid_argument("there is no builder for the ComandType of data");
	return builders[type]->create(data);
}

void ComponentFactory::addBuilder(ComponentBuilder* builder)
{
	ComponentType type = builder->getComponentType();
	if (type >= builders.size())
	{
		// warning: builders had to grow
		int prevSize = builders.size();
		builders.resize(type + 1);
		for (int i = prevSize; i < type; i++)
			builders[i] = nullptr;
	}
	if (builders[type] != nullptr)
		throw runtime_error("there is already a builder of the same ComponentType");
	builders[type] = builder;
}
