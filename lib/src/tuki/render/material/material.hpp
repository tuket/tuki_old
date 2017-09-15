#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../gl/shader.hpp"

class MaterialManager;

class MaterialTemplate
{
public:

	friend class MaterialManager;

	MaterialTemplate() {}

	const char* getName()const;

private:
	MaterialManager* manager;

	MaterialTemplate(int id, int shaderProgram);

	int id;
	int shaderProgram;

	// the name of the attribute in the shader ->
	// entry id for querying tables
	std::map<std::string, int> nameToEntry;
	// uniform ID in the shader
	std::vector<int> unifId;
	// uniform types
	std::vector<UnifType> unifType;

	// array of the fist byte offset for each unif
	std::vector<int> idEntryToDataByte;
};

class Material
{
public:

	Material() {}

	Material(const MaterialTemplate& materialTemplate);

	void setValue(unsigned entry, float value);
	void setValue(unsigned entry, const glm::vec2& value);
	void setValue(unsigned entry, const glm::vec3& value);
	void setValue(unsigned entry, const glm::vec4& value);
	void setValue(unsigned entry, int value);
	void setValue(unsigned entry, const glm::ivec2& value);
	void setValue(unsigned entry, const glm::ivec3& value);
	void setValue(unsigned entry, const glm::ivec4& value);
	void setValue(unsigned entry, const glm::mat2x3& value);
	void setValue(unsigned entry, const glm::mat3x2& value);
	void setValue(unsigned entry, const glm::mat2x4& value);
	void setValue(unsigned entry, const glm::mat4x2& value);
	void setValue(unsigned entry, const glm::mat3x4& value);
	void setValue(unsigned entry, const glm::mat4x3& value);
	void setValue(unsigned entry, const char* textureName);

private:
	int materialTemplateId;

	// one contiguous memory chunck for storing all uniform data
	char* data;

};

// manages the materials and material templates, avoids duplicates
class MaterialManager
{
public:
	MaterialManager() {}


};