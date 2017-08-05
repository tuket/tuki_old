#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum class UnifType
{
	FLOAT, VEC2, VEC3, VEC4,
	INT, INT2, INT3, INT4,
	UINT, UINT2, UINT3, UINT4,
	TEXTURE,
	COUNT
};

class MaterialTemplate
{

private:
	int shaderProgram;

	// the name of the attribute in the shader ->
	// entry id for querying tables
	std::map<std::string, int> nameToEntry;
	// uniform ID in the shader
	std::vector<int> unifId;
	// uniform types
	std::vector<UnifType> unifType;

	// DATA
	// number of attributes for each type
	std::vector<int> typeToNumEntries;
	// entry to the chunck of data
	std::vector<int> idEntryToDataEntry;


private:

	UnifType getUnifType(float) const { return UnifType::FLOAT; }
	UnifType getUnifType(glm::vec2) const { return UnifType::VEC2; }
	UnifType getUnifType(glm::vec3) const { return UnifType::VEC3; }
	UnifType getUnifType(glm::vec4) const { return UnifType::VEC4; }
	UnifType getUnifType(int) const { return UnifType::INT; }
	UnifType getUnifType(glm::ivec2) const { return UnifType::INT2; }
	UnifType getUnifType(glm::ivec3) const { return UnifType::INT3; }
	UnifType getUnifType(glm::ivec4) const { return UnifType::INT4; }
	UnifType getUnifType(unsigned) const { return UnifType::UINT; }
	UnifType getUnifType(glm::uvec2) const { return UnifType::UINT2; }
	UnifType getUnifType(glm::uvec3) const { return UnifType::UINT3; }
	UnifType getUnifType(glm::uvec4) const { return UnifType::UINT4; }
	///UnifType getUnifType(TextureId )

};

class Material
{
public:

	Material() {}

	void setUniform();


private:
	int shaderProgramId;

	// one contiguous memory chunck for storing all uniform data
	std::vector<int> data;

private:
	



};