#include "shader_pool.hpp"

#include "../gl/shader.hpp"

using namespace std;

void ShaderPool::getShaderProgram(
	const std::string& vertShadPath, const std::string& fragShadPath,
	const std::string& geomShadPath,
	AttribInitilizer attribInitializer)
{
	auto vsIt = vertShaderNameToId.find(vertShadPath);
	auto fsIt = fragShaderNameToId.find(fragShadPath);
	auto gsIt = fragShaderNameToId.find(geomShadPath);

	int vs, fs, gs;

	if (vsIt != vertShaderNameToId.end() && fsIt != fragShaderNameToId.end())
	{
		vs = vsIt->second;
		fs = vsIt->second;
		if (gsIt != geomShaderNameToId.end())
		{
			gs = gsIt->second;
			auto progIt = shadersToProgram.find({ vs, fs, gs });
			if (progIt != shadersToProgram.end())
			{
				// the program is already loaded
				return;
			}
			else
			{
				// the program is not loaded but the needed shaders are

			}
		}
	}
}

void ShaderPool::getVertShader()
{

}

void ShaderPool::getFragShader()
{

}

void ShaderPool::getGeomShader()
{

}