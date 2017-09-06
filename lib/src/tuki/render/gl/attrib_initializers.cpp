#include "attrib_initializers.hpp"

#include "shader.hpp"
#include "attribs.hpp"

namespace AttribInitilizers
{
	const AttribInitilizer generic =
		[](ShaderProgram& prog)
	{
		const int n = (int)AttribLocation::NUM_ATTRIBS;
		for (int i = 0; i < n; i++)
		{
			prog.bindAttrib(ATTRIB_NAMES[i], i);
		}
	};

	const AttribInitilizer uv =
		[](ShaderProgram& prog)
	{
		unsigned loc = (unsigned)AttribLocation::TEX_COORD;
		prog.bindAttrib(ATTRIB_NAMES[loc], loc);
	};
}
