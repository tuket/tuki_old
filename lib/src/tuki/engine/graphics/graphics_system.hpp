#pragma once

#include "../scene/component/mesh_component.hpp"
#include "../scene/component/material_component.hpp"
#include "../scene/component/point_light_component.hpp"
#include <vector>

class Renderer3D;

class GraphicsSystem
{
	friend class Engine;
	friend class SceneNode;
	
public:
	
	
private:
	Renderer3D* renderer3d;
	std::vector<MeshComponent> meshes;
	std::vector<MaterialComponent> materials;
	std::vector<PointLightComponent> pointLights;


private:
	GraphicsSystem();


	
};