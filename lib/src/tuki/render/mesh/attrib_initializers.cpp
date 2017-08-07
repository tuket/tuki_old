#include "attrib_initializers.hpp"

#include "../shader/shader.hpp"
#include "mesh.hpp"

namespace AttribInitilizers
{
	AttribInitilizer generic =
		[](ShaderProgram& prog)
	{
		const int n = (int)EAttribLocation::NUM_ATTRIBS;
		for (int i = 0; i < n; i++)
		{
			prog.bindAttrib(ATTRIB_NAMES[i], i);
		}
	};

	AttribInitilizer uv =
		[](ShaderProgram& prog)
	{
		unsigned loc = (unsigned)EAttribLocation::TEX_COORD;
		prog.bindAttrib(ATTRIB_NAMES[loc], loc);
	};
}
