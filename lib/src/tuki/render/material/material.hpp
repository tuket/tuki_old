#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>
#include <rapidjson/fwd.h>

#include "../gl/shader.hpp"
#include "../../util/singleton.hpp"

class MaterialManager;

// Material template handle.
// All the data is managed by the MaterialManager, this class is just a flyweight
class MaterialTemplate
{
public:
	MaterialTemplate() {}

	std::uint16_t getId()const;
	const char* getName()const;

private:

	// offset to the first byte from the MaterialManager allocator
	std::uint16_t id;
};

// Material handle.
// All the data is managed by the MaterialManager, this class is just a flyweight
// allows for sharing data amoung multiple objects by duplicating etc
// when a data modify is attempted, if the data is shared, it will create it's own copy
class Material
{
	friend class MaterialManager;
public:

	Material() {}

	Material(MaterialTemplate materialTemplate);

	template <typename T>
	void setValue(unsigned slot, T t);

private:

	// returns true if this is the only material instance sharing the same data
	bool isUnique()const;

	// this must be called each time the material is going to be modified
	void modifyNotification();

	// DATA
	std::uint32_t id;	// the fist 16 bits indicate the tamplate, the last 16 indicate the instance

};

// manages the materials, material templates and shaders, avoids duplicates
class MaterialManager : public Singleton<MaterialManager>
{
public:

	MaterialTemplate loadMaterialTemplate(const char* fileName);
	
	void releaseMaterialTemplate(MaterialTemplate materialTemplate);

	Material createMaterial(MaterialTemplate materialTemplate);
	Material loadMaterial(const char* fileName);

	void releaseMaterial(Material material);

	const char* getMaterialTemplateName()const;

	void makeUnique(Material& material);

private:

	MaterialManager();

	MaterialTemplate loadMaterialTemplate(rapidjson::Document& doc);

	// DATA //
	const unsigned MB = 1024 * 1024;
	const unsigned MATERIAL_TEMPLATE_CHUNK_SIZE = 4 * MB;
	const unsigned MATERIAL_CHUNK_LENGTH = 128;

	std::vector<void*> materialTemplateDataChunks;

	/* Storage for the material values
	 The outer vector has one entry for each material template
	 The inner vector will have entries depending the necesary memory space
	 The fist entry in the inner vector is the default value for the materials of the corresponding template */
	std::vector<std::vector<void*> > materialDataChunks;

	/* Table for fast lookup of material template
	 Given the material template id you get the offset to the first byte
	 Since the are several chuncks of data you will have to perform the integer division
	  ( / MATERIAL_TEMPLATE_CHUNK_SIZE) in order to get the chunk id. Then you can get
	  the actual offset inside the chunk with the ramainder ( % MATERIAL_TEMPLATE_CHUNK_SIZE) */
	std::vector<unsigned> materialTemplateOffsets;

	std::map<std::string, std::uint16_t> materialTemplateNameToId;	// the name is actually the path
	std::map<std::uint16_t, std::string> materialTemplateIdToName;

	std::vector<std::uint32_t> nextMaterialFreeSlot;

	struct MaterialTemplateEntryHeader
	{
		std::uint32_t shaderProgramId;
		std::uint16_t numSlots;
		std::uint16_t flags;
		std::uint32_t materialSize;
	};
	struct MaterialTamplateEntrySlot
	{
		std::uint16_t type;		// type of the uniform
		std::uint16_t unifLoc;	// uniform location
		char name[28];			// uniform name
	};

	struct MaterialEntryHeader
	{
		union {
			struct
			{
				//-HEADER-//
				unsigned sharedCount;

				//-BODY-//
				// material values
			}header;

			std::uint32_t nextFree;	// 16 most significant bits: chunkIndex, rest: chunkSlotIndex
		};
	};

	MaterialTemplateEntryHeader* accessMaterialTemplate(std::uint16_t mtid);

	void allocateNewMaterialChunk(std::uint16_t mtid);
};


template <typename T>
void Material::setValue(unsigned slot, T t)
{
	MaterialManager* man = MaterialManager::getSingleton();
	if (!isUnique()) man->makeUnique(this);
	man->
}