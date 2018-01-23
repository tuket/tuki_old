#include <iostream>
#include <pugixml.hpp>
#include <SDL.h>
#include <glad/glad.h>
#include <tuki/render/gl/render.hpp>
#include <tuki/render/material/material.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

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
			"test1",
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
	
	MaterialManager* materialManager = MaterialManager::getSingleton();
	Material material;
	try {
		material = materialManager->loadMaterial("materials/red_material.json");
	}
	catch (runtime_error e){
		cout << e.what() << endl;
	}

	Mesh mesh = Mesh::load("mesh/monkey.obj");
	MeshGpu meshGpu;
	MeshGpu::initFromMesh(meshGpu, mesh);

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
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ||
				event.type == SDL_QUIT) {
				run = false;
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		ShaderProgram shaderProg = material.getShaderProg();
		glm::mat4 modelMat = glm::mat4(1);
		glm::mat4 viewMat = glm::translate(glm::mat4(), glm::vec3(0, 0, -2));
		glm::mat4 projMat = glm::ortho(-2.f, 2.f, -2.f, 2.f, 0.05f, 500.f);
		glm::mat4 modelViewMat = viewMat * modelMat;
		glm::mat4 modelViewProj = projMat * modelViewMat;
		glm::mat3 normalMat = glm::inverse(glm::transpose(modelViewMat));
		shaderProg.uploadUniform("modelViewProjMat", modelViewProj);
		shaderProg.uploadUniform("modelMat", modelMat);
		shaderProg.uploadUniform("modelViewMat", modelViewMat);
		shaderProg.uploadUniform("normalMat", normalMat);
		shaderProg.uploadUniform("L", glm::vec3(0, 0, 1));
		material.use();
		meshGpu.bind();
		glDrawElements(GL_TRIANGLES, meshGpu.getNumElements(), GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);

	}

	return 0;

}
