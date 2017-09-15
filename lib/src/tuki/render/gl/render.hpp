#pragma once

#include "attrib_initializers.hpp"
#include "mesh_gpu.hpp"
#include "render_target.hpp"
#include "texture.hpp"
#include "shader.hpp"

enum class PolygonDrawMode
{
	POINT,
	LINE,
	FILL
};

struct SDL_Window;

namespace RenderApi
{
	SDL_Window* createWindowWithContext(
		const char* title,
		unsigned xPos, unsigned yPos,
		unsigned width, unsigned height);

	void draw(const IMeshGpu& mesh);

	void setClearColor(float r, float g, float b);
	void setClearColor(float r, float g, float b, float a);

	void enableDepthTest(bool yes);

	void enableFaceCulling(bool yes);

	void setPolygonDrawMode(PolygonDrawMode mode);

	void swap(SDL_Window* window);
}

