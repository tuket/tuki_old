#pragma once

#include "attrib_initializers.hpp"
#include "mesh_gpu.hpp"
#include "render_target.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include <vector>

enum class PolygonDrawMode
{
	POINT,
	LINE,
	FILL
};

struct SDL_Window;
struct BlendEquationRGB;
class BlendEquationAlpha;

namespace RenderApi
{
	SDL_Window* createWindowWithContext(
		const char* title,
		unsigned xPos, unsigned yPos,
		unsigned width, unsigned height);

	void draw(const MeshGpu& mesh);

	void setClearColor(float r, float g, float b);
	void setClearColor(float r, float g, float b, float a);
	void setClearDepth(float depth);

	void enableDepthTest(bool yes);

	void enableFaceCulling(bool yes);

	void enableBlend(bool yes);
	void enableBlend(int i, bool yes);
	void enableBlend(std::vector<bool>& yes);

	void setBlendEquations(const BlendEquationRGB& rgb, const BlendEquationAlpha& alpha);

	void setPolygonDrawMode(PolygonDrawMode mode);

	void swap(SDL_Window* window);
}

