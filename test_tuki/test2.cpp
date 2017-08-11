#include <iostream>
#include <pugixml.hpp>
#include <SDL.h>
#include <glad/glad.h>
#include <tuki/render/shader/shader.hpp>
#include <tuki/render/mesh/simple_meshes.hpp>
#include <tuki/render/mesh/attrib_initializers.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

const unsigned SCREEN_WIDTH = 800;
const unsigned SCREEN_HEIGHT = 600;

static SDL_Window* window;
static bool run;

int main(int argc, char** argv)
{
	
	cout << SDL_GetBasePath() << endl;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// don't allow deprecated GL functions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// make the window
	window =
		SDL_CreateWindow(
			"tracer",
			100, 100,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL
		);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!gladLoadGL())
	{
		cout << "gladLoadGL failed" << endl;
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;
	const GLubyte *gpuVendor = glGetString(GL_VENDOR);
	std::cout << "Graphics card: " << gpuVendor << std::endl;

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	VertexShader vertShad;
	vertShad.loadFromFile("shaders/test2.vs");
	vertShad.compile();
	if (!vertShad.hasCompiledOk())
	{
		cout << vertShad.getCompileError() << endl;
	}
	FragmentShader fragShad;
	fragShad.loadFromFile("shaders/test2.fs");
	fragShad.compile();
	if (!fragShad.hasCompiledOk())
	{
		cout << fragShad.getCompileError() << endl;
	}
	ShaderProgram prog;
	prog.init();
	prog.setVertexShader(vertShad);
	prog.setFragmentShader(fragShad);
	AttribInitilizers::generic(prog);
	prog.link();
	if (!prog.hasLinkedOk())
	{
		cout << prog.getLinkError() << endl;
	}

	Mesh cubeMesh = SimpleMeshes::createBox(0.5, 0.5, 0.5);
	MeshGpuGeneric cube;
	cube.load(cubeMesh);

	// matrices
	mat4 model = rotate(radians(90.f), vec3(1, 1, 0));
	mat4 view = mat4();
	mat4 proj = perspective(radians(60.f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.f);
	mat4 modelViewProj = proj * view * model;
	prog.uploadUniform("modelViewProj", modelViewProj);

	static float prevTime = (float)SDL_GetTicks();
	static float curTime = (float)SDL_GetTicks();
	run = true;
	while (run)
	{

		// compute delta time
		curTime = (float)SDL_GetTicks();
		float dt = (curTime - prevTime) / 1000.f;
		prevTime = curTime;

		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				run = false;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		prog.use();
		cube.bind();
		glDrawArrays(GL_TRIANGLES, 0, cubeMesh.getNumIndices());

		SDL_GL_SwapWindow(window);

	}

	return 0;

}
