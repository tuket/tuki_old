#pragma once

#include "../scene/component/mesh_component.hpp"
#include "../scene/component/material_component.hpp"
#include "../scene/component/point_light_component.hpp"
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

	// Meshes
	static const unsigned MESHES_CHUNK_SIZE = 64;
	typedef std::array<MeshComponent, MESHES_CHUNK_SIZE> MeshChunk;
	std::vector<MeshChunk*> meshes;
	MeshComponent* freeMeshSlot;

	// Materials
	static const unsigned MATERIALS_CHUNCK_SIZE = 32;
	typedef std::array<MaterialComponent, MATERIALS_CHUNCK_SIZE> MaterialChunk;
	std::vector<MaterialChunk*> materials;
	MaterialComponent* freeMaterialSlot;

	// Point Lights
	static const unsigned POINT_LIGHTS_CHUNK_SIZE = 32;
	typedef std::array<PointLightComponent, POINT_LIGHTS_CHUNK_SIZE> PointLightChunk;
	std::vector<PointLightChunk*> pointLights;
	PointLightComponent* freePointLightSlot;


private:
	GraphicsSystem();

	MeshComponent* createMeshComponent(const std::string& name);
	MaterialComponent* createMaterialComponent(const std::string& name);
	PointLightComponent* createPointLightComponent();


	
};