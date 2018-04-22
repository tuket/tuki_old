#pragma once

#include <tuki/engine/scene/component/component.hpp>
#include <vector>
#include <array>
#include <string>

class Renderer3D;

class GraphicsSystem
{
	friend class Engine;
	friend class SceneNode;
	
public:
	
	
private:
	Renderer3D* renderer3d;

private:
	GraphicsSystem();
	
};