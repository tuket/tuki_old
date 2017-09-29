#include "material.hpp"

#include <rapidjson/document.h>
#include "../../util/util.hpp"
#include <exception>
#include <sstream>
#include "shader_pool.hpp"

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
	id = mat.id;
}

// MATERIAL MANAGER

MaterialTemplate MaterialManager::loadMaterialTemplate(const string& path)
{
	string txt = loadStringFromFile(path.c_str());
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

	string vertShadName, fragShadName, geomShadName = "";

	Value::MemberIterator vertIt = shadersIt->value.FindMember("vert");
	if (vertIt == shadersIt->value.MemberEnd()) throw runtime_error("shaders/vert is mandatory");
	if (!vertIt->value.IsString()) throw runtime_error("vert shader must be string");
	vertShadName = vertIt->value.GetString();

	Value::MemberIterator fragIt = shadersIt->value.FindMember("frag");
	if (fragIt == shadersIt->value.MemberEnd()) throw runtime_error("sahders/frag is mandatory");
	if (!fragIt->value.IsString()) throw runtime_error("frag shader must be string");
	fragShadName = fragIt->value.GetString();

	Value::MemberIterator geomIt = shadersIt->value.FindMember("geom");
	if (fragIt != shadersIt->value.MemberEnd())
	{
		// geometry shaders are optional
		if(!geomIt->value.IsString()) throw runtime_error("geom shader must be string");
		geomShadName = geomIt->value.GetString();
	}

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

	ShaderPool* shaderPool = ShaderPool::getSingleton();
	
	// fill header
	MaterialTemplateEntryHeader* head = accessMaterialTemplate(mtid);
	head->numSlots = numSlots;
	head->materialSize = materialSize;
	head->shaderProgram = shaderPool->getShaderProgram(vertShadName, fragShadName, geomShadName);
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

Material MaterialManager::loadMaterial(rapidjson::Document& doc)
{
	// CCP

}

Material MaterialManager::createMaterial(MaterialTemplate materialTemplate)
{
	const uint16_t mtid = materialTemplate.getId();
	Material material;
	material.id = mtid << 16;
	return material;
}

Material MaterialManager::loadMaterial(const string& path)
{
	string txt = loadStringFromFile(path.c_str());
	Document doc;
	doc.Parse(txt.c_str());
	Material res = loadMaterial(doc);
	return res;
}

void MaterialManager::releaseMaterial(Material material)
{
	const uint32_t id = material.getId();
	const uint16_t mtid = id >> 16;
	const uint16_t mid = id & 0xFFFF;
	const unsigned slotId = mid % MATERIAL_CHUNK_LENGTH;
	const unsigned chunkId = mid / MATERIAL_CHUNK_LENGTH;
	if (mid == 0)
	{
		// 0 corresponds tho the template default value which mustn not be released
		return;
	}

	MaterialTemplateEntryHeader* templHead =  accessMaterialTemplate(mtid);
	const unsigned materialSize = templHead->materialSize;

	// insert in the free slot chain, last realeased -> fist reused
	MaterialEntryHeader* matHead = accessMaterialData(material.id);
	uint32_t nextFree = matHead->nextFree;
	matHead->nextFree = nextMaterialFreeSlot[mtid];
	nextMaterialFreeSlot[mtid] = nextFree;
}

string MaterialManager::getMaterialTemplateName(MaterialTemplate materialTemplate)const
{
	auto it = materialTemplateIdToName.find(materialTemplate.getId());
	if (it == materialTemplateIdToName.end())
	{
		throw runtime_error("the material template doesn't exist");
	}
	return it->second;
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

	char* srcChunk = (char*)chunks[mtid];
	char* pSrcSlot = &srcChunk[materialSize * mid];
	MaterialEntryHeader* prevSlotHeader = (MaterialEntryHeader*)pSrcSlot;

	// the material with mid 0 is the deafult value of the template
	// we don't want to reference count that one because it must always be there for fast copying
	if (mid != 0 && prevSlotHeader->header.sharedCount < 2)
	{
		assert(false && "the material was already unique");
		return;
	}
	
	// we use 0 for saying "there aren't free slots" because 0 is never free
	// 0 is reserved for the template default value and should never be realeased
	if ((uint16_t)nextMaterialFreeSlot[mtid] == 0) allocateNewMaterialChunk(mtid);

	uint32_t newMaterialSlot = nextMaterialFreeSlot[mtid];
	uint32_t slotIndex = newMaterialSlot & 0xFFFF;
	uint32_t chunkIndex = newMaterialSlot >> 16;
	char* dstChunk = (char*)chunks[chunkIndex];
	char* pDstSlot = &dstChunk[materialSize * slotIndex];
	MaterialEntryHeader* newSlotHeader = (MaterialEntryHeader*)pDstSlot;
	nextMaterialFreeSlot[mtid] = newSlotHeader->nextFree;

	const unsigned headerSize = sizeof(unsigned);
	memcpy(pDstSlot + headerSize, pSrcSlot + headerSize, materialSize - headerSize);

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

MaterialManager::MaterialEntryHeader* MaterialManager::accessMaterialData(uint16_t mtid, uint16_t mid)
{
	vector<void*>& v = materialDataChunks[mtid];
	const unsigned n = MATERIAL_CHUNK_LENGTH;
	MaterialTemplateEntryHeader* tempHead = accessMaterialTemplate(mtid);
	uint32_t materialSize = tempHead->materialSize;
	unsigned chunkId = mid / n;
	char* chunk = (char*)v[chunkId];
	unsigned index = mid % n;
	char* data = chunk + materialSize * index;
	return (MaterialEntryHeader*)data;
}

MaterialManager::MaterialEntryHeader* MaterialManager::accessMaterialData(uint32_t id)
{
	uint16_t mtid = id >> 16;
	uint16_t mid = id & 0x0000FFFF;
	return accessMaterialData(mtid, mid);
}

void MaterialManager::allocateNewMaterialChunk(uint16_t mtid)
{
	assert((uint16_t)nextMaterialFreeSlot[mtid] == MATERIAL_CHUNK_LENGTH &&
		"this must be called only if we have run out of memory");

	MaterialManager::MaterialTemplateEntryHeader* header =
		accessMaterialTemplate(mtid);

	const unsigned materialSlotSize = header->materialSize + sizeof(unsigned);

	char* data = new char[materialSlotSize * MATERIAL_CHUNK_LENGTH];
	materialDataChunks[mtid].push_back(data);

	const uint32_t mtid32 = mtid;
	for (unsigned i = 0; i < MATERIAL_CHUNK_LENGTH; i++)
	{
		MaterialEntryHeader* header =
			(MaterialEntryHeader*) (&data[i * materialSlotSize]);
		header->nextFree = (mtid32 << 16) || i;
	}

	nextMaterialFreeSlot[mtid] = mtid32 << 16;
}

void MaterialManager::allocateNewMaterialTemplateChunk()
{
	void* chunk = new char[MATERIAL_CHUNK_LENGTH];
	materialTemplateDataChunks.push_back(chunk);
}