#include "engine.hpp"

#include "tuki/engine/scene/component/component.hpp"
#include <stdexcept>

ComponentFactory* Engine::componentFactory = nullptr;
bool Engine::initCalled = false;

SystemId Engine::addSystem(EngineSystem* system)
{
	systems.push_back(system);
	return idMap.add(system->getName());
}

EngineSystem* Engine::getSystem(SystemId id)
{
	if (id >= systems.size())
		throw std::runtime_error("invalid SystemID");
	return systems[id];
}

SystemId Engine::getSystemId(const std::string& name)
{
	try {
		return idMap.id(name);
	}
	catch(...) {
		throw std::runtime_error("there is no system with name" + name);
	}
}

void Engine::init()
{
	if (initCalled)
		throw std::runtime_error("Engine::init called twice");

	componentFactory = new ComponentFactory();
	for (EngineSystem* system : systems)
		system->onInit();
}

ComponentFactory* Engine::getComponentFactory()
{
	return componentFactory;
}