#include "shader.hpp"

#include <assert.h>
#include "../../util/util.hpp"
#include <glad/glad.h>
#include <cassert>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "texture.hpp"
#include <map>

using namespace std;
using namespace glm;

unsigned getUnifSize(UnifType ut)
{
	const unsigned n = (unsigned)UnifType::COUNT;
	const unsigned lookUpTable[n] =
	{
		1 * sizeof(float), 2 * sizeof(float), 3 * sizeof(float), 4 * sizeof(float),
		1 * sizeof(int), 2 * sizeof(int), 3 * sizeof(int), 4 * sizeof(int),
		1 * sizeof(unsigned), 2 * sizeof(unsigned), 3 * sizeof(unsigned), 4 * sizeof(unsigned),
		2*2 * sizeof(float), 3*3 * sizeof(float), 4*4 * sizeof(float),
		2*3 * sizeof(float), 3*2 * sizeof(float),
		2*4 * sizeof(float), 4*2 * sizeof(float),
		3*4 * sizeof(float), 4*3 * sizeof(float),
	};
	unsigned i = (unsigned)ut;
	assert(i >= 0 && i < n);
	return lookUpTable[i];
}

const char* getUnifTypeName(UnifType ut)
{
	const unsigned n = (unsigned)UnifType::COUNT;
	const char const* lookUpTable[n] =
	{
		"float", "vec2", "vec3", "vec4",
		"int", "ivec2", "ivec3", "ivec4",
		"uint", "uivec2", "uivec3", "uivec4",
		"mat2", "mat3", "mat4",
		"mat2x3", "mat2x4",
		"mat2x4", "mat4x2",
		"mat3x4", "mat4x3",
	};
	unsigned i = (unsigned)ut;
	assert(i >= 0 && i < n);
	return lookUpTable[i];
}

UnifType getUnifTypeFromName(const char* name)
{
	const map<string, UnifType> lookUp =
	{
		{ "float", UnifType::FLOAT },
		{ "vec2", UnifType::VEC2 },
		{ "vec3", UnifType::VEC3 },
		{ "vec4", UnifType::VEC4 },
		{ "int", UnifType::INT },
		{ "ivec2", UnifType::INT2 },
		{ "ivec3", UnifType::INT3 },
		{ "ivec4", UnifType::INT4 },
		{ "uint", UnifType::UINT },
		{ "uivec2", UnifType::UINT2 },
		{ "uivec3", UnifType::UINT3 },
		{ "uivec4", UnifType::UINT4 },
		{ "mat2", UnifType::MATRIX_2 },
		{ "mat3", UnifType::MATRIX_3 },
		{ "mat4", UnifType::MATRIX_4 },
		{ "mat2x3", UnifType::MATRIX_2x3 },
		{ "mat3x2", UnifType::MATRIX_3x2 },
		{ "mat2x4", UnifType::MATRIX_2x4 },
		{ "mat4x2", UnifType::MATRIX_4x2 },
		{ "mat3x4", UnifType::MATRIX_3x4 },
		{ "mat4x3", UnifType::MATRIX_4x3 }
	};

	map<string, UnifType>::const_iterator it = lookUp.find(name);
	if (it == lookUp.end())
	{
		throw runtime_error("not recognized unif name type: " + string(name));
	}

	return it->second;
}

// SHADER

void ShaderObject::loadFromString(const char* src)
{
	// the shader has to be loaded only once, otherwise there will be memory leaks
	assert(shaderId < 0 && "The shader has been already loaded");

	glShaderSource(shaderId, 1, &src, 0);
}

void ShaderObject::loadFromFile(const char* fileName)
{
	string src = loadStringFromFile(fileName);
	loadFromString(src.c_str());
}

void ShaderObject::compile()
{
	glCompileShader(shaderId);

	GLint compiled;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint logLen;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen);
		string logString(logLen, ' ');
		glGetShaderInfoLog(shaderId, logLen, NULL, &logString[0]);
		glDeleteShader(shaderId);
		throw logString;
	}
}

void ShaderObject::destroy()
{
	assert(shaderId >= 0 && "Attempted to destroy shader before creating it");

	glDeleteShader(shaderId);
}

void VertexShaderObject::create() { shaderId = glCreateShader(GL_VERTEX_SHADER); }
void FragmentShaderObject::create() { shaderId = glCreateShader(GL_FRAGMENT_SHADER); }
void GeometryShaderObject::create() { shaderId = glCreateShader(GL_GEOMETRY_SHADER); }


// SHADER PROGRAM

void ShaderProgram::create()
{
	program = glCreateProgram();
}

void ShaderProgram::bindAttrib(const char* name, int loc)
{
	glBindAttribLocation(program, loc, name);
}

void ShaderProgram::setVertexShader(VertexShaderObject vertShad)
{
	glAttachShader(program, vertShad.getId());
}

void ShaderProgram::setFragmentShader(FragmentShaderObject fragShad)
{
	glAttachShader(program, fragShad.getId());
}

void ShaderProgram::setGeometryShader(GeometryShaderObject geomShad)
{
	glAttachShader(program, geomShad.getId());
}

void ShaderProgram::link()
{
	glLinkProgram(program);

	// check if the linking failed
	int linkedOk;
	glGetProgramiv(program, GL_LINK_STATUS, &linkedOk);
	if (!linkedOk)
	{
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		string str = string(len, ' ');
		glGetProgramInfoLog(program, len, NULL, &str[0]);
		throw str;
	}
}

void ShaderProgram::use()
{
	assert(program >= 0 && "Attempted to use an invalid shader program");

	glUseProgram(program);
}

void ShaderProgram::useProgram()
{
	glUseProgram(program);
}

void ShaderProgram::free()
{
	assert(program >= 0 && "Attempted to free an invalid shader program");

	glDeleteProgram(program);
}

int ShaderProgram::getUniformLocation(const char* name)const
{
	return glGetUniformLocation(program, name);
}
// UNIFORM UPLOADERS
void ShaderProgram::uploadUniform(int location, float value)
{
	glUniform1f(location, value);
}

void ShaderProgram::uploadUniform(int location, const vec2& value)
{
	glUniform2fv(location, 1, &value[0]);
}

void ShaderProgram::uploadUniform(int location, const vec3& value)
{
	glUniform3fv(location, 1, &value[0]);
}

void ShaderProgram::uploadUniform(int location, const vec4& value)
{
	glUniform4fv(location, 1, &value[0]);
}

void ShaderProgram::uploadUniform(int location, int value)
{
	glUniform1i(location, value);
}

void ShaderProgram::uploadUniform(int location, const ivec2& value)
{
	glUniform2iv(location, 1, &value[0]);
}

void ShaderProgram::uploadUniform(int location, const ivec3& value)
{
	glUniform3iv(location, 1, &value[0]);
}

void ShaderProgram::uploadUniform(int location, const ivec4& value)
{
	glUniform4iv(location, 1, &value[0]);
}

void ShaderProgram::uploadUniform(int location, const mat2& value)
{
	glUniformMatrix2fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat2x3& value)
{
	glUniformMatrix2x3fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat3x2& value)
{
	glUniformMatrix3x2fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat2x4& value)
{
	glUniformMatrix2x4fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat4x2& value)
{
	glUniformMatrix4x2fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat3x4& value)
{
	glUniformMatrix3x4fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, const mat4x3& value)
{
	glUniformMatrix4x3fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::uploadUniform(int location, TextureUnit value)
{
	glUniform1i(location, (int)value);
}
