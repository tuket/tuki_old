#pragma once

#include "mesh/attrib_initializers.hpp"
#include "mesh/mesh.hpp"
#include "texture/render_target.hpp"
#include "texture/texture.hpp"
#include "shader/shader.hpp"

namespace Render
{
	void draw(const IMeshGpu& mesh);

	void setClearColor(float r, float g, float b);

	void enableDepthTest(bool yes);

	void enableFaceCulling(bool yes);
}

