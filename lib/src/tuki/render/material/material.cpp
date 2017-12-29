#include "material.hpp"

#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/error/en.h>
#include "../../util/util.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include "shader_pool.hpp"
#include "../../util/multi_sort.hpp"
#include <glm/common.hpp>

using namespace std;
using namespace rapidjson;
using namespace glm;

// MATERIAL TEMPLATE

std::uint16_t MaterialTemplate::getId()const
{
	return id;
}

string MaterialTemplate::getName()const
{
	MaterialManager* man = MaterialManager::getSingleton();
	return man->getMaterialTemplateName(*this);
}

void MaterialTemplate::bindProgram()
{
	MaterialManager* man = MaterialManager::getSingleton();
	man->bindMaterialTemplateProgram(*this);
}

// MATERIAL

Material::Material(MaterialTemplate materialTemplate)
{
	MaterialManager* man = MaterialManager::getSingleton();
	Material mat = man->createMaterial(materialTemplate);
	id = mat.id;
}

void Material::use()
{
	MaterialManager* man = MaterialManager::getSingleton();
	man->useMaterial(*this);
}

void Material::useBatched()
{
	MaterialManager* man = MaterialManager::getSingleton();
	man->useMaterialBatched(*this);
}

bool Material::isUnique()const
{
	MaterialManager* man = MaterialManager::getSingleton();
	return man->isUnique(*this);
}

void Material::modifyNotification()
{
	MaterialManager* man = MaterialManager::getSingleton();
	if (!man->isUnique(*this)) man->makeUnique(*this);
}

// MATERIAL MANAGER

MaterialManager::MaterialManager()
{
	// fist allocation
	allocateNewMaterialTemplateChunk();
	nextMaterialTemplateOffset = 0;
}

MaterialTemplate MaterialManager::loadMaterialTemplate(const string& path)
{
	MaterialTemplate existing = getMaterialTemplate(path);
	if (existing.id != 0xFFFF) return existing;

	string txt = loadStringFromFile(path.c_str());
	Document doc;
	doc.Parse(txt.c_str());
	MaterialTemplate res = loadMaterialTemplate(doc);
	materialTemplateNameToId[path] = res.id;
	materialTemplateIdToName[res.id] = path;

	return res;
}

MaterialTemplate MaterialManager::getMaterialTemplate(const string& path)const
{
	const auto it = materialTemplateNameToId.find(path);
	MaterialTemplate res;
	if (it == materialTemplateNameToId.end())
	{
		res.id = -1;
	}
	else
	{
		res.id = it->second;
	}
	return res;
}

MaterialTemplate MaterialManager::loadMaterialTemplate(rapidjson::Document& doc)
{
	ShaderPool* shaderPool = ShaderPool::getSingleton();
	const unsigned maxSlotNameSize = sizeof(MaterialTemplateEntrySlot::name);

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
	if (fragIt == shadersIt->value.MemberEnd()) throw runtime_error("shaders/frag is mandatory");
	if (!fragIt->value.IsString()) throw runtime_error("frag shader must be string");
	fragShadName = fragIt->value.GetString();

	Value::MemberIterator geomIt = shadersIt->value.FindMember("geom");
	if (geomIt != shadersIt->value.MemberEnd())
	{
		// geometry shaders are optional
		if(!geomIt->value.IsString()) throw runtime_error("geom shader must be string");
		geomShadName = geomIt->value.GetString();
	}

	vector<string> slotNames;
	vector<UnifType> types;
	vector<Value::MemberIterator> sortedIts;
	for (Value::MemberIterator it = slotsIt->value.MemberBegin();
		it != slotsIt->value.MemberEnd();
		it++)
	{
		if (!it->name.IsString()) throw runtime_error("slot names must be strings");
		string name = it->name.GetString();

		if (name.size() >= maxSlotNameSize)
		{
			stringstream ss;
			ss << maxSlotNameSize - 1;
			throw runtime_error("attrib name is longer than " + ss.str() + " characters");
		}

		slotNames.push_back(name);

		Value::MemberIterator typeIt = it->value.FindMember("type");
		if (typeIt == it->value.MemberEnd()) throw runtime_error("type is mandatory for slots");
		if (!typeIt->value.IsString()) throw runtime_error("slot types must be strings");
		UnifType type = getUnifTypeFromName(typeIt->value.GetString());
		types.push_back(type);

		sortedIts.push_back(it);
	}

	sortVectorsAscending(slotNames, slotNames, types, sortedIts);

	const unsigned numSlots = slotNames.size();
	unsigned materialSize = 0;
	for (UnifType type : types) materialSize += getUnifSize(type);

	uint16_t mtid = materialTemplateOffsets.size();
	allocateMaterialTemplate(numSlots);
	
	// fill header
	MaterialTemplateEntryHeader* head = accessMaterialTemplate(mtid);
	head->numSlots = numSlots;
	head->materialSize = materialSize + sizeof(MaterialEntryHeader::header);
	ShaderProgram shaderProgram =
		head->shaderProgram =
		shaderPool->getShaderProgram(vertShadName, fragShadName, geomShadName);
	head->flags; // TODO
	
	// fill slots
	MaterialTemplateEntrySlot* slots = (MaterialTemplateEntrySlot*)(head + 1);
	unsigned offset = 0;
	for (unsigned i = 0; i < numSlots; i++)
	{
		const string& name = slotNames[i];
		
		strncpy(slots[i].name, slotNames[i].c_str(), maxSlotNameSize);

		slots[i].type = types[i];
		slots[i].offset = offset;
		slots[i].unifLoc = shaderProgram.getUniformLocation(name.c_str());

		offset += getUnifSize(types[i]);
	}

	// default values
	nextMaterialFreeSlot.push_back(MATERIAL_CHUNK_LENGTH);
	materialDataChunks.push_back(vector<void*>());
	allocateNewMaterialChunk(mtid);

	MaterialEntryHeader* matHead = accessMaterialData(((uint32_t)mtid) << 16);
	matHead->header.sharedCount = -1;

	for (unsigned i = 0; i < numSlots; i++)
	{
		Value::MemberIterator it = sortedIts[i];
		Value::MemberIterator defIt = it->value.FindMember("default");
		if (defIt != it->value.MemberEnd())
		{
			// parse specified default
			parseJsonValueAndSet(defIt->value, i, matHead, head);
		}
		else
		{
			// use the default default: just zero
			const unsigned size = getUnifSize(types[i]);
			MaterialTemplateEntrySlot* templSlots = (MaterialTemplateEntrySlot*)&head[1];
			unsigned offset = templSlots[i].offset;
			char* data = (char*)&matHead[1];
			data = data + offset;
			memset(data, 0, size);
		}
	}
	nextMaterialFreeSlot[mtid] = 1;

	MaterialTemplate templ;
	templ.id = mtid;
	return templ;
}

Material MaterialManager::loadMaterial(rapidjson::Document& doc)
{
	assert(doc.IsObject());
	Value::MemberIterator templateIt = doc.FindMember("template");
	if (templateIt == doc.MemberEnd()) throw runtime_error("missing 'template' member");

	if (!templateIt->value.IsString()) throw runtime_error("'template' must be string");
	string templatePath = templateIt->value.GetString();

	MaterialTemplate templ = loadMaterialTemplate(templatePath);
	MaterialTemplateEntryHeader* templHead = accessMaterialTemplate(templ.id);
	
	Material mat;
	Value::MemberIterator slotsIt = doc.FindMember("slots");
	if (slotsIt == doc.MemberEnd())
	{
		mat = createMaterial(templ);
		return mat;
	}
	if (!slotsIt->value.IsObject()) throw runtime_error("'slots' must be an object");
	Value::Object slotsObj = slotsIt->value.GetObject();

	mat = duplicateMaterialAndMakeUnique(templ.id << 16);	// duplicate the default value
	
	MaterialEntryHeader* matHead = accessMaterialData(mat.id);
	char* matData = (char*)&matHead[1];

	for (Value::MemberIterator it = slotsObj.MemberBegin();
		it != slotsObj.MemberEnd();
		++it)
	{
		if (!it->name.IsString()) throw runtime_error("slot names must be string");
		string slotName = it->name.GetString();
		uint16_t slot = nameToSlot(slotName, templHead);
		parseJsonValueAndSet(it->value, slot, matHead, templHead);
	}
	return mat;
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
	if (doc.HasParseError())
	{
		string errorMsg = GetParseError_En(doc.GetParseError());
		throw runtime_error(errorMsg);
	}
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

bool MaterialManager::isUnique(const Material& mat)const
{
	const MaterialEntryHeader* matHead = accessMaterialData(mat.getId());
	return matHead->header.sharedCount == 1;
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

	uint32_t srcSlotIndex = mid % MATERIAL_CHUNK_LENGTH;
	uint32_t srcChunkIndex = mid / MATERIAL_CHUNK_LENGTH;
	char* srcChunk = (char*)chunks[srcChunkIndex];
	char* pSrcSlot = &srcChunk[materialSize * srcSlotIndex];
	MaterialEntryHeader* srcSlotHeader = (MaterialEntryHeader*)pSrcSlot;

	// the material with mid 0 is the deafult value of the template
	// we don't want to reference count that one because it must always be there for fast copying
	if (mid != 0 && srcSlotHeader->header.sharedCount < 2)
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
	MaterialEntryHeader* dstSlotHeader = (MaterialEntryHeader*)pDstSlot;
	nextMaterialFreeSlot[mtid] = dstSlotHeader->nextFree;

	const unsigned headerSize = sizeof(unsigned);
	memcpy(pDstSlot + headerSize, pSrcSlot + headerSize, materialSize - headerSize);

	srcSlotHeader->header.sharedCount--;
	dstSlotHeader->header.sharedCount = 1;

	material.id = mtid | (chunkIndex * MATERIAL_CHUNK_LENGTH + slotIndex);
}

const MaterialManager::MaterialTemplateEntryHeader* MaterialManager::accessMaterialTemplate(std::uint16_t mtid)const
{
	const unsigned offset = materialTemplateOffsets[mtid];
	const unsigned chunkIndex = offset / MATERIAL_TEMPLATE_CHUNK_SIZE;
	const unsigned chunkOffset = offset % MATERIAL_TEMPLATE_CHUNK_SIZE;
	char* chunk = (char*)materialTemplateDataChunks[chunkIndex];
	return (MaterialManager::MaterialTemplateEntryHeader*) (&chunk[chunkOffset]);
}

MaterialManager::MaterialTemplateEntryHeader* MaterialManager::accessMaterialTemplate(std::uint16_t mtid)
{
	return
		const_cast<MaterialTemplateEntryHeader*>(
			static_cast<const MaterialManager*>(this)->accessMaterialTemplate(mtid)
		);
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

const MaterialManager::MaterialEntryHeader* MaterialManager::accessMaterialData(uint16_t mtid, uint16_t mid)const
{
	const vector<void*>& v = materialDataChunks[mtid];
	const unsigned n = MATERIAL_CHUNK_LENGTH;
	const MaterialTemplateEntryHeader* tempHead = accessMaterialTemplate(mtid);
	uint32_t materialSize = tempHead->materialSize;
	unsigned chunkId = mid / n;
	char* chunk = (char*)v[chunkId];
	unsigned index = mid % n;
	char* data = chunk + materialSize * index;
	return (MaterialEntryHeader*)data;
}

const MaterialManager::MaterialEntryHeader* MaterialManager::accessMaterialData(uint32_t id)const
{
	uint16_t mtid = id >> 16;
	uint16_t mid = id & 0x0000FFFF;
	return accessMaterialData(mtid, mid);
}

MaterialManager::MaterialEntryHeader* MaterialManager::accessMaterialData(uint32_t id)
{
	uint16_t mtid = id >> 16;
	uint16_t mid = id & 0x0000FFFF;
	return accessMaterialData(mtid, mid);
}

MaterialManager::MaterialEntryHeader* MaterialManager::accessMaterialData(uint16_t mtid, uint16_t mid)
{
	return
		const_cast<MaterialEntryHeader*>(
				static_cast<const MaterialManager*>(this)->accessMaterialData(mtid, mid)
		);
}

void MaterialManager::allocateNewMaterialChunk(uint16_t mtid)
{
	assert((uint16_t)nextMaterialFreeSlot[mtid] == MATERIAL_CHUNK_LENGTH &&
		"this must be called only if we have run out of memory");

	MaterialManager::MaterialTemplateEntryHeader* header =
		accessMaterialTemplate(mtid);

	const unsigned materialSlotSize = header->materialSize;
	const uint32_t chunkId = materialDataChunks[mtid].size() << 16;

	char* data = new char[materialSlotSize * MATERIAL_CHUNK_LENGTH];
	materialDataChunks[mtid].push_back(data);

	for (unsigned i = 0; i < MATERIAL_CHUNK_LENGTH-1; i++)
	{
		MaterialEntryHeader* header =
			(MaterialEntryHeader*) (&data[i * materialSlotSize]);
		header->nextFree = chunkId | (i + 1);
	}
	{
		MaterialEntryHeader* header =
			(MaterialEntryHeader*)(&data[(MATERIAL_CHUNK_LENGTH-1) * materialSlotSize]);
		header->nextFree = 0;
	}

	nextMaterialFreeSlot[mtid] = chunkId;
}

void MaterialManager::allocateNewMaterialTemplateChunk()
{
	void* chunk = new char[MATERIAL_CHUNK_LENGTH];
	materialTemplateDataChunks.push_back(chunk);
}

Material MaterialManager::duplicateMaterialAndMakeUnique(std::uint32_t id)
{
	const uint16_t mtid = id >> 16;
	const uint16_t mid = id & 0xFFFF;

	MaterialTemplateEntryHeader* templHead = accessMaterialTemplate(mtid);
	const unsigned materialSize = templHead->materialSize;

	vector<void*>& chunks = materialDataChunks[mtid];
	uint32_t srcSlotIndex = mid % MATERIAL_CHUNK_LENGTH;
	uint32_t srcChunkIndex = mid / MATERIAL_CHUNK_LENGTH;
	char* srcChunk = (char*)chunks[srcChunkIndex];
	char* pSrcSlot = &srcChunk[materialSize * srcSlotIndex];
	MaterialEntryHeader* srcSlotHeader = (MaterialEntryHeader*)pSrcSlot;

	// we use 0 for saying "there aren't free slots" because 0 is never free
	// 0 is reserved for the template default value and should never be realeased
	if ((uint16_t)nextMaterialFreeSlot[mtid] == 0) allocateNewMaterialChunk(mtid);

	uint32_t newMaterialSlot = nextMaterialFreeSlot[mtid];
	uint32_t slotIndex = newMaterialSlot & 0xFFFF;
	uint32_t chunkIndex = newMaterialSlot >> 16;
	char* dstChunk = (char*)chunks[chunkIndex];
	char* pDstSlot = &dstChunk[materialSize * slotIndex];
	MaterialEntryHeader* dstSlotHeader = (MaterialEntryHeader*)pDstSlot;
	nextMaterialFreeSlot[mtid] = dstSlotHeader->nextFree;

	const unsigned headerSize = sizeof(unsigned);
	memcpy(pDstSlot + headerSize, pSrcSlot + headerSize, materialSize - headerSize);

	dstSlotHeader->header.sharedCount = 1;

	uint32_t newMid = slotIndex + MATERIAL_CHUNK_LENGTH * chunkIndex;
	Material material;
	material.id = ((uint32_t)mtid) << 16 | newMid;
	return material;
}

void MaterialManager::useMaterial(const Material& material)
{
	uint16_t mtid = material.id >> 16;
	uint16_t mid = (uint16_t)material.id;
	MaterialTemplateEntryHeader* head = accessMaterialTemplate(mtid);
	head->shaderProgram.use();
	useMaterialBatched(mtid, mid);
}

void MaterialManager::useMaterialBatched(const Material& material)
{
	uint16_t mtid = material.id >> 16;
	uint16_t mid = (uint16_t)material.id;
	useMaterialBatched(mtid, mid);
}

void MaterialManager::useMaterialBatched(uint16_t mtid, uint16_t mid)
{
	MaterialTemplateEntryHeader* templHead = accessMaterialTemplate(mtid);
	MaterialEntryHeader* matHead = accessMaterialData(mid);
	const unsigned n = templHead->numSlots;
	MaterialTemplateEntrySlot* templSlots = (MaterialTemplateEntrySlot*)&templHead[1];
	ShaderProgram& prog = templHead->shaderProgram;
	char* data = (char*)&matHead[1];

	for (unsigned slot = 0; slot < n; slot++)
	{
		UnifType type = templSlots[slot].type;
		unsigned offset = templSlots[slot].offset;
		unsigned loc = templSlots[slot].unifLoc;
		char* slotData = data + offset;

		prog.uploadUniformData(type, loc, slotData);
	}
}

Material MaterialManager::duplicateMaterialAndMakeUnique(Material material)
{
	return duplicateMaterialAndMakeUnique(material.id);
}

void parseJsonArrayAndSet(
	char* dataOutput,
	UnifType type,
	const Value::ConstArray& a
)
{
	static int xdata[64];

	UnifType basicType = getUnifBasicType(type);
	unsigned n = getUnifNumElems(type);

	unsigned count = 0;
	for (Value::ConstValueIterator it = a.Begin(); it != a.End(); ++it)
	{
		if (count >= n)
		{
			throw runtime_error("too many elements");
		}

		if (basicType == UnifType::FLOAT)
		{
			float* x = (float*)xdata;
			if (it->IsFloat())
			{
				x[count] = it->GetFloat();
			}
			else if (it->IsInt())
			{
				x[count] = it->GetInt();
			}
			else{
				stringstream ss;
				ss << "[" << count << "]" + string(" must be float");
				throw runtime_error(ss.str());
			}
		}
		else if (basicType == UnifType::INT)
		{
			int* x = xdata;
			if (!it->IsInt())
			{
				stringstream ss;
				ss << "[" << count << "]" + string(" must be int");
				throw runtime_error(ss.str());
			}
			x[count] = it->GetInt();
		}
		else if (basicType == UnifType::UINT)
		{
			unsigned* x = (unsigned*)xdata;
			if (!it->IsUint())
			{
				stringstream ss;
				ss << "[" << count << "]" + string(" must be unsigned");
				throw runtime_error(ss.str());
			}
			x[count] = it->GetUint();
		}
		else
		{
			assert(false && "basic type not implemented");
		}
		
		count++;
	}
	if (count < n) throw runtime_error(string("too few elements"));

	copy(xdata, &xdata[n], (int*)dataOutput);
}

void MaterialManager::parseJsonValueAndSet(
	const Value& val,
	unsigned slot,
	MaterialEntryHeader* matHead,
	MaterialTemplateEntryHeader* templHead
)
{
	MaterialTemplateEntrySlot* templSlots = (MaterialTemplateEntrySlot*)&templHead[1];
	UnifType type = templSlots[slot].type;

	unsigned offset = templSlots[slot].offset;
	char* data = (char*)&matHead[1];
	data = data + offset;

	if (type == UnifType::FLOAT)
	{
		if (!val.IsFloat()) throw runtime_error(templSlots[slot].name + string(" must be float"));

		float x[1] = { val.GetFloat() };
		copy(x, &x[1], data);
	}
	else if (type == UnifType::INT)
	{
		if(!val.IsInt()) throw runtime_error(templSlots[slot].name + string(" must be int"));

		int x[1] = { val.GetInt() };
		copy(x, &x[1], data);
	}
	else if (type == UnifType::UINT)
	{
		if (!val.IsUint()) throw runtime_error(templSlots[slot].name + string(" must be unisgned"));

		unsigned x[1] = { val.GetUint() };
		copy(x, &x[1], data);
	}
	else if (type == UnifType::TEXTURE)
	{
		// TODO: texture manager
	}
	else	// vectors and materices
	{
		if (!val.IsArray()) throw runtime_error(templSlots[slot].name + string(" must be an array"));
		Value::ConstArray a = val.GetArray();
		parseJsonArrayAndSet(data, type, a);
	}

}

// performs a binary search because slots are sorted by name
uint16_t MaterialManager::nameToSlot(const string& name, MaterialTemplateEntryHeader* head)const
{
	const MaterialTemplateEntrySlot* slots = (const MaterialTemplateEntrySlot*)&head[1];
	const unsigned n = head->numSlots;
	unsigned i, j, ij;
	i = 0;
	j = n - 1;
	while (i <= j)
	{
		ij = (i + j) / 2;
		string ijName = slots[ij].name;
		if (name == ijName) return ij;
		else
		{
			if (name < ijName) j = ij - 1;
			else			   i = ij + 1;
		}
	}
	throw runtime_error("slot name '" + name + "' does not exist");
}

void MaterialManager::bindMaterialTemplateProgram(MaterialTemplate& templ)
{
	MaterialTemplateEntryHeader* head = accessMaterialTemplate(templ.id);
	head->shaderProgram.use();
}
