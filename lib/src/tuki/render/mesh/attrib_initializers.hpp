#pragma once

#include <functional>

class ShaderProgram;
typedef std::function<void(ShaderProgram& prog)> AttribInitilizer;

namespace AttribInitilizers
{
	extern AttribInitilizer generic;
	extern AttribInitilizer uv;
}
