#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <string>
#include <numeric>

enum class TextureUnit;

typedef int ShaderId;

enum class ShaderType
{
	VERTEX = 0,
	FRAGMENT,
	GEOMETRY,
};

enum class UnifType : std::uint16_t
{
	FLOAT, VEC2, VEC3, VEC4,
	INT, INT2, INT3, INT4,
	UINT, UINT2, UINT3, UINT4,
	MATRIX_2, MATRIX_3, MATRIX_4,
	MATRIX_2x3, MATRIX_3x2,
	MATRIX_2x4, MATRIX_4x2,
	MATRIX_3x4, MATRIX_4x3,

	COUNT,

	TEXTURE = INT,
};

unsigned getUnifSize(UnifType ut);
const char* getUnifTypeName(UnifType ut);
UnifType getUnifTypeFromName(const char* name);

// SHADER
class ShaderObject
{
public:

protected:

	ShaderObject() : shaderId(-1){}

public:

	ShaderId getId()const { return shaderId; }

	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);

	void compile();

	void destroy();

protected:
	ShaderId shaderId;
};

// VERTEX SHADER
class VertexShaderObject : public ShaderObject
{
public:
	VertexShaderObject() : ShaderObject() {}
	void create();
};

// FRAGMENT SHADER
class FragmentShaderObject : public ShaderObject
{
public:
	FragmentShaderObject() : ShaderObject() {}
	void create();
};

// GEOMETRY SHADER
class GeometryShaderObject : public ShaderObject
{
public:
	GeometryShaderObject() : ShaderObject() {}
	void create();
};


// SHADER PROGRAM
class ShaderProgram
{
public:
	ShaderProgram() : program(-1){}

	void setVertexShader(VertexShaderObject vertShad);
	void setFragmentShader(FragmentShaderObject fragShad);
	void setGeometryShader(GeometryShaderObject geomShad);

	// these functions are intended to be used in order
	void create();
	// setShaders()
	void bindAttrib(const char* name, int loc);
	void link();
	void use();
	void free();

	int getUniformLocation(const char* name)const;
	// uniform uploaders
	static void uploadUniform(int location, float value);
	static void uploadUniform(int location, const glm::vec2& value);
	static void uploadUniform(int location, const glm::vec3& value);
	static void uploadUniform(int location, const glm::vec4& value);
	static void uploadUniform(int location, int value);
	static void uploadUniform(int location, const glm::ivec2& value);
	static void uploadUniform(int location, const glm::ivec3& value);
	static void uploadUniform(int location, const glm::ivec4& value);
	static void uploadUniform(int location, const glm::mat2& value);
	static void uploadUniform(int location, const glm::mat3& value);
	static void uploadUniform(int location, const glm::mat4& value);
	static void uploadUniform(int location, const glm::mat2x3& value);
	static void uploadUniform(int location, const glm::mat3x2& value);
	static void uploadUniform(int location, const glm::mat2x4& value);
	static void uploadUniform(int location, const glm::mat4x2& value);
	static void uploadUniform(int location, const glm::mat3x4& value);
	static void uploadUniform(int location, const glm::mat4x3& value);
	static void uploadUniform(int location, TextureUnit value);
	template <typename T>
	static void uploadUniform(const char* name, const T& value)
	{
		int loc = getUniformLocation(name);
		useProgram();
		uploadUniform(loc, value);
	}

protected:
	int program;

	void useProgram();

};

inline UnifType getUnifType(float) { return UnifType::FLOAT; }
inline UnifType getUnifType(glm::vec2) { return UnifType::VEC2; }
inline UnifType getUnifType(glm::vec3) { return UnifType::VEC3; }
inline UnifType getUnifType(glm::vec4) { return UnifType::VEC4; }
inline UnifType getUnifType(int) { return UnifType::INT; }
inline UnifType getUnifType(glm::ivec2) { return UnifType::INT2; }
inline UnifType getUnifType(glm::ivec3) { return UnifType::INT3; }
inline UnifType getUnifType(glm::ivec4) { return UnifType::INT4; }
inline UnifType getUnifType(unsigned) { return UnifType::UINT; }
inline UnifType getUnifType(glm::uvec2) { return UnifType::UINT2; }
inline UnifType getUnifType(glm::uvec3) { return UnifType::UINT3; }
inline UnifType getUnifType(glm::uvec4) { return UnifType::UINT4; }
inline UnifType getUnifType(glm::mat2) { return UnifType::MATRIX_2; }
inline UnifType getUnifType(glm::mat3) { return UnifType::MATRIX_3; }
inline UnifType getUnifType(glm::mat4) { return UnifType::MATRIX_4; }
inline UnifType getUnifType(glm::mat2x3) { return UnifType::MATRIX_2x3; }
inline UnifType getUnifType(glm::mat3x2) { return UnifType::MATRIX_3x2; }
inline UnifType getUnifType(glm::mat2x4) { return UnifType::MATRIX_2x4; }
inline UnifType getUnifType(glm::mat4x2) { return UnifType::MATRIX_4x2; }
inline UnifType getUnifType(glm::mat3x4) { return UnifType::MATRIX_3x4; }
inline UnifType getUnifType(glm::mat4x3) { return UnifType::MATRIX_4x3; }
