#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>

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
	std::uint32_t id;

};

// manages the materials, material templates and shaders, avoids duplicates
class MaterialManager : public Singleton<MaterialManager>
{
public:
	MaterialTemplate createMaterialTemplate();
	MaterialTemplate loadMaterialTemplate();
	
	void releaseMaterialTemplate(MaterialTemplate materialTemplate);

	Material createMaterial(MaterialTemplate materialTemplate);
	Material loadMaterial();

	void releaseMaterial(Material material);

	const char* getMaterialTemplateName()const;

	void makeUnique(Material& material);

private:

	MaterialManager();

	// DATA //
	const unsigned MB = 1024 * 1024;
	const unsigned MATERIAL_TEMPLATE_CHUNK_SIZE = 4 * MB;
	const unsigned MATERIAL_CHUNK_LENGTH = 128;
	std::vector<void*> materialTemplateDataChunks;
	std::vector<std::vector<void*> > materialDataChunks;
	std::vector<unsigned> materialTemplateOffsets;

	std::vector<std::uint32_t> nextMaterialFreeSlot;

	struct MaterialTemplateEntryHeader
	{
		//-HEADER-//
		// This is the header every material template has
		std::uint16_t shaderProgramId;	
		std::uint16_t numSlots;
		std::uint32_t materialSize;
		char name[56];

		//-BODY-//
		// for each slot
		//     - type (2 byte)
		//     - unifid (2 byte)
		//     - name (60 bytes)
	};
	struct MaterialTamplateEntrySlot
	{
		std::uint16_t type;
		std::uint16_t unifId;
		char name[60];
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