#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>
#include <rapidjson/fwd.h>
#include <array>

#include "../gl/shader.hpp"
#include "../../util/singleton.hpp"

class MaterialManager;

// Material template handle.
// All the data is managed by the MaterialManager, this class is just a flyweight
class MaterialTemplate
{
	friend class MaterialManager;
public:
	MaterialTemplate() {}

	std::uint16_t getId()const;
	std::string getName()const;

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

	std::uint16_t getTemplateId()const { return id >> 16; }
	std::uint16_t getInstanceId()const { return id & 0x0000FFFF; }
	std::uint32_t getId()const { return id; }

	template <typename T>
	void setValue(unsigned slot, T val);

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

	// get the material template if loaded, pthorwise the id will be -1
	MaterialTemplate getMaterialTemplate(const std::string& path)const;

	// load the material file from a file, if has been already loaded returns the same object
	MaterialTemplate loadMaterialTemplate(const std::string& path);
	
	//void releaseMaterialTemplate(MaterialTemplate materialTemplate); // TODO?

	Material createMaterial(MaterialTemplate materialTemplate);
	Material loadMaterial(const std::string& path);

	void releaseMaterial(Material material);

	std::string getMaterialTemplateName(MaterialTemplate materialTemplate)const;

	void makeUnique(Material& material);

	template <typename T>
	void setMaterialValue(Material& material, unsigned slot, T val);

	template <typename T>
	void setMaterialValueUnsafe(const Material& material, unsigned slot, T val);

private:

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
	unsigned nextMaterialTemplateOffset;

	std::map<std::string, std::uint16_t> materialTemplateNameToId;	// the name is actually the path
	std::map<std::uint16_t, std::string> materialTemplateIdToName;

	std::vector<std::uint32_t> nextMaterialFreeSlot;

	// TYPES //
	struct MaterialTemplateEntryHeader
	{
		ShaderProgram shaderProgram;
		std::uint16_t numSlots;
		std::uint16_t flags;
		std::uint32_t materialSize;
	};
	struct MaterialTemplateEntrySlot	// < sorted by name!
	{
		static const unsigned NAME_MAX_SIZE = 24;
		UnifType type;		// type of the uniform
		std::uint16_t unifLoc;	// uniform location
		std::uint16_t offset;	// offset within the material
		char name[NAME_MAX_SIZE];			// uniform name
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

	// FUNCTIONS //
	MaterialManager();

	MaterialTemplate loadMaterialTemplate(rapidjson::Document& doc);
	Material loadMaterial(rapidjson::Document& doc);

	Material duplicateMaterialAndMakeUnique(std::uint32_t id);
	Material duplicateMaterialAndMakeUnique(Material material);

	MaterialTemplateEntryHeader* accessMaterialTemplate(std::uint16_t mtid);
	MaterialTemplateEntryHeader* allocateMaterialTemplate(unsigned numSlots);

	MaterialEntryHeader* accessMaterialData(std::uint16_t mtid, std::uint16_t mid);
	MaterialEntryHeader* accessMaterialData(std::uint32_t id);

	void allocateNewMaterialChunk(std::uint16_t mtid);
	void allocateNewMaterialTemplateChunk();

	void parseJsonValueAndSet(
		const rapidjson::Value& val,
		unsigned slot,
		MaterialEntryHeader* matHead,
		MaterialTemplateEntryHeader* templHead
	);

	uint16_t nameToSlot(const std::string& name, MaterialTemplateEntryHeader* head)const;

};


template <typename T>
void Material::setValue(unsigned slot, T val)
{
	MaterialManager* man = MaterialManager::getSingleton();
	man->setMaterialValue(*this, slot, val);
}

template <typename T>
void MaterialManager::setMaterialValue(Material& material, unsigned slot, T val)
{
	if (!material.isUnique()) makeUnique(material);
	setMaterialValueUnsafe(material, slot, val);
}

template <typename T>
void MaterialManager::setMaterialValueUnsafe(const Material& material, unsigned slot, T val)
{
	uint16_t mtid = material.getTemplateId();
	uint16_t mid = material.getInstanceId();
	MaterialTemplateEntryHeader* tempHead = accessMaterialTemplate(mtid);
	MaterialTemplateEntrySlot* tempSlot = (MaterialTemplateEntrySlot*)tempHead[1];
	tempSlot = &tempSlot[slot];
	unsigned slotOffset = tempSlot->offset;
	MaterialEntryHeader* matHead = accessMaterialData(mtid, mid);
	char* data = (char*)&matHead[1];
	data = data + slotOffset;
	*((T*)data) = val;
}