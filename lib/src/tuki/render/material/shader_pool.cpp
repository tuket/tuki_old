#include "shader_pool.hpp"

#include "../gl/shader.hpp"

using namespace std;

ShaderProgram ShaderPool::getShaderProgram(
	const std::string& vertShadPath, const std::string& fragShadPath,
	const std::string& geomShadPath,
	AttribInitilizer attribInitializer)
{
	auto vsIt = vertShaderNameToId.find(vertShadPath);
	auto fsIt = fragShaderNameToId.find(fragShadPath);
	auto gsIt = fragShaderNameToId.find(geomShadPath);

	int vs, fs, gs = -1;

	if (vsIt != vertShaderNameToId.end() && fsIt != fragShaderNameToId.end())
	{
		vs = vsIt->second;
		fs = vsIt->second;
		if (geomShadPath == "" ||
		   (geomShadPath != "" && gsIt != geomShaderNameToId.end()))
		{
			gs = gsIt->second;
			auto progIt = shadersToProgram.find({ vs, fs, gs });
			if (progIt != shadersToProgram.end())
			{
				// the program is already loaded
				return programs[progIt->second];
			}
		}
	}
	
	// create vertex shader if needed
	if (vsIt != vertShaderNameToId.end())
	{
		vs = vsIt->second;
	}
	else
	{
		VertexShaderObject vertShad;
		vertShad.create();
		vertShad.loadFromFile(vertShadPath.c_str());
		vertShad.compile();
		vs = vertShaders.size();
		vertShaders.push_back(vertShad);
	}

	// create fragment shader id needed
	if (fsIt != fragShaderNameToId.end())
	{
		fs = fsIt->second;
	}
	else
	{
		FragmentShaderObject fragShad;
		fragShad.create();
		fragShad.loadFromFile(fragShadPath.c_str());
		fragShad.compile();
		fs = fragShaders.size();
		fragShaders.push_back(fragShad);
	}

	// create geometry shader id needed
	if (geomShadPath != "")
	{
		if (gsIt != geomShaderNameToId.end())
		{
			gs = gsIt->second;
		}
		else
		{
			GeometryShaderObject geomShad;
			geomShad.create();
			geomShad.loadFromFile(geomShadPath.c_str());
			geomShad.compile();
			gs = geomShaders.size();
			geomShaders.push_back(geomShad);
		}
	}

	// create shader program
	ShaderProgram prog;
	prog.create();

	prog.setVertexShader(vertShaders[vs]);
	prog.setFragmentShader(fragShaders[fs]);
	if (gs != -1) prog.setGeometryShader(geomShaders[gs]);

	AttribInitilizers::generic(prog);

	prog.link();

	return prog;
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