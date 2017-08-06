#include "shader.hpp"

#include <assert.h>
#include "../../util/util.hpp"
#include <glad/glad.h>
#include <cassert>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// SHADER

void Shader::loadFromString(const char* src, unsigned type)
{
	// the shader has to be loaded only once, otherwise there will be memory leaks
	assert(shaderId < 0 && "The shader has been already loaded");

	shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &src, 0);

	status = Status::LOADED;
}

void Shader::loadFromFile(const char* fileName, unsigned type)
{
	try
	{
		string src = loadStringFromFile(fileName);
		loadFromString(src.c_str());
		status = Status::LOADED;
	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
		status = Status::LOAD_ERROR;
	}
}

void Shader::compile()
{
	assert(status == Status::LOADED && "Attempted to compile shader before loading source");

	glCompileShader(shaderId);

	GLint compiled;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		status = Status::COMPILE_ERROR;
	}

	status = Status::COMPILED;
}

string Shader::getCompileError()const
{
	assert(status == Status::COMPILE_ERROR && "Attempted to get compile error of a shader that didn't fail compiling");

	GLint logLen;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen);
	string logString(logLen, ' ');
	glGetShaderInfoLog(shaderId, logLen, NULL, &logString[0]);
	glDeleteShader(shaderId);
	return logString;
}

void Shader::destroy()
{
	assert(shaderId >= 0 && "Attempted to destroy shader before creating it");

	glDeleteShader(shaderId);
	status = Status::DESTROYED;
}

// VERTEX SHADER

void VertexShader::loadFromString(const char* src)
{
	Shader::loadFromString(src, GL_VERTEX_SHADER);
}

void VertexShader::loadFromFile(const char* fileName)
{
	Shader::loadFromFile(fileName, GL_VERTEX_SHADER);
}

// FRAGMENT SHADER

void FragmentShader::loadFromString(const char* src)
{
	Shader::loadFromString(src, GL_FRAGMENT_SHADER);
}

void FragmentShader::loadFromFile(const char* fileName)
{
	Shader::loadFromFile(fileName, GL_FRAGMENT_SHADER);
}

// GEOMETRY SHADER
void GeometryShader::loadFromString(const char* src)
{
	Shader::loadFromString(src, GL_GEOMETRY_SHADER);
}

void GeometryShader::loadFromFile(const char* fileName)
{
	Shader::loadFromFile(fileName, GL_GEOMETRY_SHADER);
}


// SHADER PROGRAM

bool ShaderProgram::link()
{
	const bool compiledOk = vertShad.hasCompiledOk() && fragShad.hasCompiledOk() &&
		(!hasGeometryShader() || (hasGeometryShader() && geomShad.hasCompiledOk()));
	assert(compiledOk && "Attempted to link shader program with uncompiled objects");

	glLinkProgram(program);
	int linkedOk;
	glGetProgramiv(program, GL_LINK_STATUS, &linkedOk);

	// check if the linking failed
	if (!linkedOk)
	{
		// print error
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		char *logString = new char[len];
		glGetProgramInfoLog(program, len, NULL, logString);
		cout << "Error: " << logString << endl;
		delete[] logString;

		// delete the program
		// note: we don't delete shader objects because they
		// could be beeing used by another program
		glDeleteProgram(program);
		program = -1;

		return false;
	}

	if (vertShad.getId() >= 0) glDetachShader(program, vertShad.getId());
	if (fragShad.getId() >= 0) glDetachShader(program, fragShad.getId());
	if (geomShad.getId() >= 0) glDetachShader(program, geomShad.getId());

	return true;
}

void ShaderProgram::use()
{
	assert(program >= 0 && "Attempted to use an invalid shader program");

	glUseProgram(program);
}

void ShaderProgram::free()
{
	assert(program >= 0 && "Attempted to free an invalid shader program");

	glDeleteProgram(program);
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