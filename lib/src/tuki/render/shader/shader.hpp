#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

typedef int ShaderId;

enum class EShaderType
{
	VERTEX = 0,
	FRAGMENT,
	GEOMETRY,
};

enum class EUnifType
{
	FLOAT, VEC2, VEC3, VEC4,
	INT, INT2, INT3, INT4,
	UINT, UINT2, UINT3, UINT4,
	TEXTURE = INT,
	MATRIX_2, MATRIX_3, MATRIX_4,
	MATRIX_2x3, MATRIX_3x2,
	MATRIX_2x4, MATRIX_4x2,
	MATRIX_3x4, MATRIX_4x3,
	COUNT
};

// SHADER
class Shader
{

protected:

	Shader() : shaderId(-1) {}

	void loadFromString(const char* src, unsigned type);
	void loadFromFile(const char* fileName, unsigned type);

public:
	
	virtual void loadFromString(const char* src) = 0;
	virtual void loadFromFile(const char* fileName) = 0;

	bool isLoaded() const { return shaderId >= 0; }

	ShaderId getId() const { return shaderId; }

	// returns false on failure
	bool compile();

	void destroy();

protected:
	ShaderId shaderId;
};

// VERTEX SHADER
class VertexShader : public Shader
{
public:
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};

// FRAGMENT SHADER
class FragmentShader : public Shader
{
public:
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};

// GEOMETRY SHADER
class GeometryShader : public Shader
{
public:
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};


// SHADER PROGRAM
class ShaderProgram
{
public:
	
	ShaderProgram() : program(-1), compiledOk(false) {}

	void setVertexShader(VertexShader vertShad) { this->vertShad = vertShad; }
	void setFragmentShader(FragmentShader fragShad) { this->fragShad = fragShad;  }
	void setGeometryShader(GeometryShader geomShad) { this->geomShad = geomShad; }

	bool hasGeometryShader() { return geomShad.isLoaded(); }

	// return false if fail
	bool compile();
	bool link();

	void use();

	// uniform uploaders
	void uploadUniform(int location, float value);
	void uploadUniform(int location, const glm::vec2& value);
	void uploadUniform(int location, const glm::vec3& value);
	void uploadUniform(int location, const glm::vec4& value);
	void uploadUniform(int location, int value);
	void uploadUniform(int location, const glm::ivec2& value);
	void uploadUniform(int location, const glm::ivec3& value);
	void uploadUniform(int location, const glm::ivec4& value);
	void uploadUniform(int location, const glm::mat2& value);
	void uploadUniform(int location, const glm::mat3& value);
	void uploadUniform(int location, const glm::mat4& value);
	void uploadUniform(int location, const glm::mat2x3& value);
	void uploadUniform(int location, const glm::mat3x2& value);
	void uploadUniform(int location, const glm::mat2x4& value);
	void uploadUniform(int location, const glm::mat4x2& value);
	void uploadUniform(int location, const glm::mat3x4& value);
	void uploadUniform(int location, const glm::mat4x3& value);


protected:

	int program;
	VertexShader vertShad;
	FragmentShader fragShad;
	GeometryShader geomShad;

	bool compiledOk;
};

inline EUnifType getUnifType(float) { return EUnifType::FLOAT; }
inline EUnifType getUnifType(glm::vec2) { return EUnifType::VEC2; }
inline EUnifType getUnifType(glm::vec3) { return EUnifType::VEC3; }
inline EUnifType getUnifType(glm::vec4) { return EUnifType::VEC4; }
inline EUnifType getUnifType(int) { return EUnifType::INT; }
inline EUnifType getUnifType(glm::ivec2) { return EUnifType::INT2; }
inline EUnifType getUnifType(glm::ivec3) { return EUnifType::INT3; }
inline EUnifType getUnifType(glm::ivec4) { return EUnifType::INT4; }
inline EUnifType getUnifType(unsigned) { return EUnifType::UINT; }
inline EUnifType getUnifType(glm::uvec2) { return EUnifType::UINT2; }
inline EUnifType getUnifType(glm::uvec3) { return EUnifType::UINT3; }
inline EUnifType getUnifType(glm::uvec4) { return EUnifType::UINT4; }
inline EUnifType getUnifType(glm::mat2) { return EUnifType::MATRIX_2; }
inline EUnifType getUnifType(glm::mat3) { return EUnifType::MATRIX_3; }
inline EUnifType getUnifType(glm::mat4) { return EUnifType::MATRIX_4; }
inline EUnifType getUnifType(glm::mat2x3) { return EUnifType::MATRIX_2x3; }
inline EUnifType getUnifType(glm::mat3x2) { return EUnifType::MATRIX_3x2; }
inline EUnifType getUnifType(glm::mat2x4) { return EUnifType::MATRIX_2x4; }
inline EUnifType getUnifType(glm::mat4x2) { return EUnifType::MATRIX_4x2; }
inline EUnifType getUnifType(glm::mat3x4) { return EUnifType::MATRIX_3x4; }
inline EUnifType getUnifType(glm::mat4x3) { return EUnifType::MATRIX_4x3; }
