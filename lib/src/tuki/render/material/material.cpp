#include "material.hpp"

#include "rapidjson/document.h"
#include "../../util/util.hpp"
#include <exception>
#include <sstream>

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
	if (vertIt == shadersIt->value.MemberEnd()) throw runtime_error("shaders/vert is mandatory");
	Value::MemberIterator fragIt = shadersIt->value.FindMember("frag");
	if (fragIt == shadersIt->value.MemberEnd()) throw runtime_error("sahders/frag is mandatory");
	Value::MemberIterator geomIt = shadersIt->value.FindMember("geom");
	// geometry shaders are optional

	vector<string> slotNames;
	vector<UnifType> types;
	vector<Value::MemberIterator> defaultValueIts;
	for (Value::MemberIterator it = slotsIt->value.MemberBegin();
		it != slotsIt->value.MemberEnd();
		it++)
	{
		if (!it->name.IsString()) throw runtime_error("slot names must be strings");
		string name = it->name.GetString();
		slotNames.push_back(name);

		Value::MemberIterator typeIt = it->value.FindMember("type");
		if (typeIt == it->value.MemberEnd()) throw runtime_error("type is mandatory for slots");
		if (!typeIt->value.IsString()) throw runtime_error("slot types must be strings");
		UnifType type = getUnifTypeFromName(typeIt->value.GetString());
		types.push_back(type);

		Value::MemberIterator defIt = it->value.FindMember("default");
		defaultValueIts.push_back(defIt);
	}

	const unsigned numSlots = slotNames.size();
	unsigned materialSize = 0;
	for (UnifType type : types) materialSize += getUnifSize(type);

	uint16_t mtid = materialTemplateOffsets.size();
	allocateMaterialTemplate(numSlots);
	
	// fill header
	MaterialTemplateEntryHeader* head = accessMaterialTemplate(mtid);
	head->numSlots = numSlots;
	head->materialSize = materialSize;
	head->shaderProgramId; // TODO;
	head->flags; // TODO
	
	// fill slots
	MaterialTemplateEntrySlot* slots = (MaterialTemplateEntrySlot*)(head + 1);
	for (unsigned i=0; i<numSlots; i++)
	{
		const unsigned maxSlotNameSize = sizeof(MaterialTemplateEntrySlot::name);
		if (slotNames[i].size() >= maxSlotNameSize)
		{
			stringstream ss;
			ss << maxSlotNameSize - 1;
			throw runtime_error("attrib name is longer than " + ss.str() + " characters");
		}
		strncpy(slots[i].name, slotNames[i].c_str(), maxSlotNameSize);

		slots[i].type = types[i];

		slots[i].unifLoc; // TODO
	}

	MaterialTemplate templ;
	templ.id = mtid;
	return templ;
}

void MaterialManager::makeUnique(Material& material)
{
	uint32_t id = material.id;
	uint16_t mtid = id >> 16;
	uint16_t mid = id;

	MaterialManager::MaterialTemplateEntryHeader* header =
		accessMaterialTemplate(mtid);

	MaterialTemplateEntrySlot* slots = (MaterialTemplateEntrySlot*) &header[1];

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

MaterialManager::MaterialTemplateEntryHeader* MaterialManager::allocateMaterialTemplate(unsigned numSlots)
{
	const unsigned requiredSpace = sizeof(MaterialTemplateEntryHeader) + numSlots * sizeof(MaterialTemplateEntrySlot);
	const unsigned chunkSize = MATERIAL_TEMPLATE_CHUNK_SIZE;
	if (requiredSpace > chunkSize) throw runtime_error("too many slots for material template");

	unsigned remainingSpaceInChunk = chunkSize - nextMaterialTemplateOffset;
	if (remainingSpaceInChunk < requiredSpace)
	{
		allocateNewMaterialTemplateChunk();
		nextMaterialTemplateOffset = (materialTemplateDataChunks.size() - 1) * chunkSize;
	}

	materialTemplateOffsets.push_back(nextMaterialTemplateOffset);

	nextMaterialTemplateOffset += requiredSpace;
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

void MaterialManager::allocateNewMaterialTemplateChunk()
{
	void* chunk = new char[MATERIAL_CHUNK_LENGTH];
	materialTemplateDataChunks.push_back(chunk);
}