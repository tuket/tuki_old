#pragma once

#include <vector>

class EngineSystem;
class ComponentFactory;

class Engine
{
	std::vector<EngineSystem*> systems;
	ComponentFactory* componentFactory;
public:
	void addSystem(EngineSystem* system);
	void init();
	ComponentFactory* getComponentFactory();
};

class EngineSystem
{
public:
	void onAdded(Engine* engine);
	void onInit();
	void onUpdate(float dt);
};