#pragma once

#include "../../util/singleton.hpp"
#include <vector>
#include <map>
#include <string>
#include <array>
#include "../gl/attrib_initializers.hpp"


class ShaderPool : public Singleton<ShaderPool>
{
public:
	void getShaderProgram(
		const std::string& vertShadPath, const std::string& fragShadPath,
		const std::string& geomShadPath = "",
		AttribInitilizer attribInitializer = AttribInitilizers::generic);

	void getVertShader();
	void getFragShader();
	void getGeomShader();

private:
	ShaderPool();

	// DATA
	std::vector<int> vertShaders;
	std::vector<int> fragShaders;
	std::vector<int> geomShaders;
	std::vector<int> programs;

	std::vector<std::array<int, 3> > programToShaders;

	std::map<std::string, int> vertShaderNameToId;
	std::map<std::string, int> fragShaderNameToId;
	std::map<std::string, int> geomShaderNameToId;
	std::map<std::array<int, 3>, int> shadersToProgram;
};