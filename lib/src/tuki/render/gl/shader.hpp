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
class Shader
{
public:
	enum class Status
	{
		START,
		LOADED,
		LOAD_ERROR,
		COMPILED,
		COMPILE_ERROR,
		DESTROYED = START
	};

protected:

	Shader() : shaderId(-1), status(Status::START) {}

	void loadFromString(const char* src, unsigned type);
	void loadFromFile(const char* fileName, unsigned type);

public:
	Status getStatus()const { return status; }
	ShaderId getId()const { return shaderId; }

	virtual void loadFromString(const char* src) = 0;
	virtual void loadFromFile(const char* fileName) = 0;
	bool isLoaded()const { return status == Status::LOADED; }

	void compile();
	bool hasCompiledOk()const { return status == Status::COMPILED; }
	std::string getCompileError()const;

	void destroy();

protected:
	ShaderId shaderId;
	Status status;
};

// VERTEX SHADER
class VertexShader : public Shader
{
public:
	VertexShader() : Shader() {}
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};

// FRAGMENT SHADER
class FragmentShader : public Shader
{
public:
	FragmentShader() : Shader() {}
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};

// GEOMETRY SHADER
class GeometryShader : public Shader
{
public:
	GeometryShader() : Shader() {}
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};


// SHADER PROGRAM
class ShaderProgram
{
public:

	enum class Status
	{
		START,
		LINKED,
		LINK_ERROR,
		DESTROYED = START
	};
	
	ShaderProgram() : program(-1){}

	void setVertexShader(VertexShader vertShad);
	void setFragmentShader(FragmentShader fragShad);
	void setGeometryShader(GeometryShader geomShad);

	bool hasGeometryShader() { return geomShad.isLoaded(); }

	Status getStatus()const { return status; }
	bool hasLinkedOk()const { return status == Status::LINKED; }
	std::string getLinkError()const;

	// these 4 function are intended to be used in order
	void init();
	void bindAttrib(const char* name, int loc);
	void link();
	void use();
	void free();

	int getUniformLocation(const char* name)const;
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
	void uploadUniform(int location, TextureUnit value);
	template <typename T>
	void uploadUniform(const char* name, const T& value)
	{
		int loc = getUniformLocation(name);
		useProgram();
		uploadUniform(loc, value);
	}


protected:

	int program;
	VertexShader vertShad;
	FragmentShader fragShad;
	GeometryShader geomShad;
	Status status;

private:
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
