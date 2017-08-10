#pragma once

#include <functional>

/*
Attribute initializers take as input a shader program and
bind the locations of the attributes. Each attribute name has
associated some location (or id), see ATTRIB_NAMES.
One could use the generic initilizer one for everything but
you can also make custom initializers to save computations.
*/

class ShaderProgram;
typedef std::function<void(ShaderProgram& prog)> AttribInitilizer;

namespace AttribInitilizers
{
	extern AttribInitilizer generic;
	extern AttribInitilizer uv;
}
