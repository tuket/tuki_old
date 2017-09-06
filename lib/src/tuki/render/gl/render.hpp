#pragma once

#include "attrib_initializers.hpp"
#include "mesh_gpu.hpp"
#include "render_target.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace Render
{
	void draw(const IMeshGpu& mesh);

	void setClearColor(float r, float g, float b);

	void enableDepthTest(bool yes);

	void enableFaceCulling(bool yes);
}

