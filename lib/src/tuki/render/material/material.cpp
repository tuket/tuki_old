#include "material.hpp"

#include "rapidjson/document.h"
#include "../../util/util.hpp"
#include <exception>

using namespace std;
using namespace rapidjson;

// MATERIAL TEMPLATE

std::uint16_t MaterialTemplate::getId()const
{
	return id;
}

const char* MaterialTemplate::getName()const
{
	MaterialManager* man = MaterialManager::getSingleton();
	return man->getMaterialTemplateName();
}

// MATERIAL

Material::Material(MaterialTemplate materialTemplate)
{
	MaterialManager* man = MaterialManager::getSingleton();
	Material mat = man->createMaterial(materialTemplate);
}

// MATERIAL MANAGER

MaterialTemplate MaterialManager::loadMaterialTemplate(const char* fileName)
{
	string txt = loadStringFromFile(fileName);
	Document doc;
	doc.Parse(txt.c_str());
	MaterialTemplate res = loadMaterialTemplate(doc);
	return res;
}

MaterialTemplate MaterialManager::loadMaterialTemplate(rapidjson::Document& doc)
{
	Value::MemberIterator shadersIt = doc.FindMember("shaders");
	if (shadersIt == doc.MemberEnd()) throw runtime_error("missing 'shaders' member");
	Value::MemberIterator slotsIt = doc.FindMember("slots");
	if (slotsIt == doc.MemberEnd()) throw runtime_error("missing 'slots' member");

	Value::MemberIterator vertIt = shadersIt->value.FindMember("vert");
	if (vertIt == shadersIt->value.MemberEnd) throw runtime_error("shaders/vert is mandatory");
	Value::MemberIterator fragIt = shadersIt->value.FindMember("frag");
	if (fragIt == shadersIt->value.MemberEnd) throw runtime_error("sahders/frag is mandatory");
	Value::MemberIterator geomIt = shadersIt->value.FindMember("geom");
	// geometry sahders are optional


}

void MaterialManager::makeUnique(Material& material)
{
	uint32_t id = material.id;
	uint16_t mtid = id >> 16;
	uint16_t mid = id;

	MaterialManager::MaterialTemplateEntryHeader* header =
		accessMaterialTemplate(mtid);

	MaterialTamplateEntrySlot* slots = (MaterialTamplateEntrySlot*) &header[1];

	const unsigned materialSize = header->materialSize;
	vector<void*>& chunks = materialDataChunks[mtid];

	char* prevChunk = (char*)chunks[mtid];
	char* pSrcSlot = &prevChunk[materialSize * mid];
	MaterialEntryHeader* prevSlotHeader = (MaterialEntryHeader*)pSrcSlot;
	if (prevSlotHeader->header.sharedCount < 2)
	{
		assert(false && "the material was already unique");
		return;
	}
	
	uint16_t nextInvalid = MaterialManager::MATERIAL_CHUNK_LENGTH;
	if ((uint16_t)nextMaterialFreeSlot[mtid] == nextInvalid) allocateNewMaterialChunk(mtid);

	uint32_t newMaterialSlot = nextMaterialFreeSlot[mtid];
	uint16_t newMid = newMaterialSlot;
	char* newChunk = (char*)chunks[mtid];
	char* pDstSlot = &newChunk[materialSize * newMid];
	MaterialEntryHeader* newSlotHeader = (MaterialEntryHeader*)pDstSlot;
	nextMaterialFreeSlot[mtid] = newSlotHeader->nextFree;

	const unsigned headerSize = sizeof(unsigned);
	memcpy(pDstSlot + headerSize, pSrcSlot + headerSize, materialSize);

	prevSlotHeader->header.sharedCount--;
	newSlotHeader->header.sharedCount = 1;
}

MaterialManager::MaterialTemplateEntryHeader* MaterialManager::accessMaterialTemplate(std::uint16_t mtid)
{
	const unsigned offset = materialTemplateOffsets[mtid];
	const unsigned chunkIndex = offset / MATERIAL_TEMPLATE_CHUNK_SIZE;
	const unsigned chunkOffset = offset % MATERIAL_TEMPLATE_CHUNK_SIZE;
	char* chunk = (char*)materialTemplateDataChunks[chunkIndex];
	return (MaterialManager::MaterialTemplateEntryHeader*) chunk[offset];
}

void MaterialManager::allocateNewMaterialChunk(uint16_t mtid)
{
	assert((uint16_t)nextMaterialFreeSlot[mtid] == MATERIAL_CHUNK_LENGTH &&
		"this must be called only if we have run out of memory");

	MaterialManager::MaterialTemplateEntryHeader* header =
		accessMaterialTemplate(mtid);

	const unsigned materialSlotSize = header->materialSize + sizeof(unsigned);

	void* data = new char[materialSlotSize * MATERIAL_CHUNK_LENGTH];
	materialDataChunks[mtid].push_back(data);

	for (unsigned i = 0; i < MATERIAL_CHUNK_LENGTH; i++)
	{
		MaterialEntryHeader* header =
			(MaterialEntryHeader*)&materialDataChunks[i * materialSlotSize];
		uint32_t mtid32 = mtid;
		header->nextFree = (mtid32 << 16) || i;
	}
}