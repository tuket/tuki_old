#include <iostream>
#include <pugixml.hpp>
#include <SDL.h>
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <tuki/render/render.hpp>
#include <tuki/render/mesh/simple_meshes.hpp>

using namespace std;
using namespace glm;

const unsigned SCREEN_WIDTH = 800;
const unsigned SCREEN_HEIGHT = 600;

static SDL_Window* window;
static bool run;

struct KeysStatus
{
	bool w, a, s, d;
}keysStatus;

int mouseX, mouseY, prevMouseX, prevMouseY;
bool mousePressed;

vec3 camPos(0, 0, 0);
float camHeading = 0;

void cameraMovement(float dt);

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
			"test4",
			100, 100,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL
		);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGL())
	{
		cout << "gladLoadGL failed" << endl;
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;
	const GLubyte *gpuVendor = glGetString(GL_VENDOR);
	std::cout << "Graphics card: " << gpuVendor << std::endl;

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	VertexShader vertShad;
	vertShad.loadFromFile("shaders/test4.vs");
	vertShad.compile();
	if (!vertShad.hasCompiledOk())
	{
		cout << vertShad.getCompileError() << endl;
	}
	FragmentShader fragShad;
	fragShad.loadFromFile("shaders/test4.fs");
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

	// render target
	RenderTarget renderTarget(1, SCREEN_WIDTH, SCREEN_HEIGHT, TexelFormat::RGBA8, true);

	// cube
	Mesh cubeMesh = SimpleMeshes::createBox(0.5, 0.5, 0.5);
	MeshGpuGeneric cube;
	cube.load(cubeMesh);

	// texture
	Texture axisTex = Texture::loadFromFile("textures/axis.png");
	prog.uploadUniform("colorTex", TextureUnit::COLOR);

	// matrices
	mat4 model = translate(vec3(0, 0, -2));
	mat4 view = translate(-camPos);
	mat4 proj = perspective(radians(60.f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.f);
	mat4 modelViewProj = proj * view * model;
	prog.uploadUniform("modelViewProj", modelViewProj);

	// init mouse status
	SDL_PumpEvents();
	mousePressed = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
	prevMouseX = mouseX;
	prevMouseY = mouseY;

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
				event.type == SDL_QUIT)
			{
				run = false;
			}
		}

		const Uint8* keys = SDL_GetKeyboardState(0);
		keysStatus.w = keys[SDL_SCANCODE_W];
		keysStatus.a = keys[SDL_SCANCODE_A];
		keysStatus.s = keys[SDL_SCANCODE_S];
		keysStatus.d = keys[SDL_SCANCODE_D];

		prevMouseX = mouseX;
		prevMouseY = mouseY;
		mousePressed = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);

		cameraMovement(dt);

		view = rotate(camHeading, vec3(0, 1, 0)) * translate(-camPos);
		modelViewProj = proj * view * model;
		prog.uploadUniform("modelViewProj", modelViewProj);

		// draw cube faces img
		renderTarget.bind();
		axisTex.bindToUnit(TextureUnit::COLOR);
		renderTarget.clear();
		prog.use();
		cube.bind();
		Render::draw(cube);
		
		static bool doneOnce = false;
		if (!doneOnce)
		{
			renderTarget.getTexture(0).save("img.png");
			doneOnce = true;
		}

		// draw scene
		RenderTarget::bindDefault();
		RenderTarget::clearDefault();
		renderTarget.getTexture(0).bindToUnit(TextureUnit::COLOR);
		cube.bind();
		Render::draw(cube);

		SDL_GL_SwapWindow(window);
	}

	return 0;

}

void cameraMovement(float dt)
{
	const float SPEED = 1.f;
	const float MOUSE_SENSITIVITY = 0.01f;

	// rotation with mouse
	if (mousePressed)
	{
		float dx = mouseX - prevMouseX;
		camHeading += dx * MOUSE_SENSITIVITY;
	}

	// translation with keyboard
	mat3 rot = rotate(-camHeading, vec3(0, 1, 0));
	vec3 forward(0, 0, -1);
	forward = rot * forward;
	vec3 right(1, 0, 0);
	right = rot * right;

	vec3 move(0);
	if (keysStatus.w)
	{
		move += forward;
	}
	if (keysStatus.s)
	{
		move -= forward;
	}
	if (keysStatus.a)
	{
		move -= right;
	}
	if (keysStatus.d)
	{
		move += right;
	}

	if(length2(move) > 1.01f) move = normalize(move);

	camPos += SPEED * move * dt;
}