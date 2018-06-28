#pragma once

#include <vector>
#include "tuki/util/id_map.hpp"

class EngineSystem;
class ComponentFactory;
typedef SystemId;

class Engine
{
	static IdMap idMap;
	static std::vector<EngineSystem*> systems;
	static ComponentFactory* componentFactory;
	static bool initCalled;
public:
	static SystemId addSystem(EngineSystem* system);
	static EngineSystem* getSystem(SystemId id);
	static SystemId getSystemId(const std::string& name);
	static void init();
	static ComponentFactory* getComponentFactory();
};

class EngineSystem
{
public:
	virtual std::string getName()const = 0;
	virtual void onInit() {}
	virtual void onUpdate(float dt) = 0;
};