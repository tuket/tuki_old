#pragma once

#include "../../util/singleton.hpp"
#include <vector>
#include <map>
#include <string>
#include <array>
#include "../gl/attrib_initializers.hpp"
#include "../gl/shader.hpp"

class ShaderPool : public Singleton<ShaderPool>
{
public:
	ShaderProgram getShaderProgram(
		const std::string& vertShadPath, const std::string& fragShadPath,
		const std::string& geomShadPath = "",
		AttribInitilizer attribInitializer = AttribInitilizers::generic);

	void getVertShader();
	void getFragShader();
	void getGeomShader();

private:
	ShaderPool();

	// DATA
	std::vector<VertexShaderObject> vertShaders;
	std::vector<FragmentShaderObject> fragShaders;
	std::vector<GeometryShaderObject> geomShaders;
	std::vector<ShaderProgram> programs;

	std::map<std::string, int> vertShaderNameToId;
	std::map<std::string, int> fragShaderNameToId;
	std::map<std::string, int> geomShaderNameToId;
	std::map<std::array<int, 3>, int> shadersToProgram;
	std::vector<std::array<int, 3> > programToShaders;
};