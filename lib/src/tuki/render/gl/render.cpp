#include "render.hpp"

#include <glad/glad.h>

#include "../mesh/mesh.hpp"
#include "mesh_gpu.hpp"

const GLenum TO_GL_GEOM_TYPE[(int)GeomType::COUNT] =
{
	GL_POINTS,
	GL_LINES,
	GL_TRIANGLES,
	GL_LINE_STRIP,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
};

namespace Render
{

void draw(const IMeshGpu& mesh)
{
	const unsigned numElements = mesh.getNumElements();
	GeomType geomType = mesh.getGeomType();
	if (mesh.hasIndices())
	{
		glDrawElements
		(
			TO_GL_GEOM_TYPE[(int)geomType],
			numElements,
			GL_UNSIGNED_INT,
			0
		);
	}
	else
	{
		glDrawArrays
		(
			TO_GL_GEOM_TYPE[(int)geomType],
			0,
			numElements
		);
	}
}

}

