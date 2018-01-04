#include "render.hpp"

#include <glad/glad.h>

#include "../mesh/mesh.hpp"
#include "mesh_gpu.hpp"
#include <iostream>
#include <SDL.h>

using namespace std;

const GLenum TO_GL_GEOM_TYPE[(int)GeomType::COUNT] =
{
	GL_POINTS,
	GL_LINES,
	GL_TRIANGLES,
	GL_LINE_STRIP,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
};

namespace RenderApi
{

static SDL_GLContext context;

SDL_Window* createWindowWithContext(
	const char* title,
	unsigned xPos, unsigned yPos,
	unsigned width, unsigned height)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// don't allow deprecated GL functions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// make the window
	SDL_Window* window =
		SDL_CreateWindow(
			title,
			xPos, yPos,
			width, height,
			SDL_WINDOW_OPENGL
		);

	context = SDL_GL_CreateContext(window);
	//SDL_GL_SetSwapInterval(1);

	if (!gladLoadGL())
	{
		cout << "gladLoadGL failed" << endl;
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;
	const GLubyte *gpuVendor = glGetString(GL_VENDOR);
	std::cout << "Graphics card: " << gpuVendor << std::endl;

	return window;
}

void init()
{
	
}

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

void setClearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 0.f);
}

void setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void setClearDepth(float depth)
{
	glClearDepth(depth);
}

void enableDepthTest(bool yes)
{
	if (yes) glEnable(GL_DEPTH_TEST);
	else	 glDisable(GL_DEPTH_TEST);
}

void enableFaceCulling(bool yes)
{
	if (yes) glEnable(GL_CULL_FACE);
	else	 glDisable(GL_CULL_FACE);
}

void setPolygonDrawMode(PolygonDrawMode mode)
{
	switch (mode)
	{
	case PolygonDrawMode::POINT:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case PolygonDrawMode::LINE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case PolygonDrawMode::FILL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		assert("not recognized polygon draw mode");
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void swap(SDL_Window* window)
{
	SDL_GL_SwapWindow(window);
}

}

