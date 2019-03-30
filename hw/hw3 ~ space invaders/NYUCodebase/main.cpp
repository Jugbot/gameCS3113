#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define STB_IMAGE_IMPLEMENTATION

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "TextureAtlas.h"
#include "Services.h"
#include "helpers.h"

SDL_Window* displayWindow;

void setup(void);
void update(void);
void display(void);

int main(int argc, char *argv[])
{
	printf("Debugging Window:\n");
	setup();

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		update();
		display();
	}

	SDL_Quit();
	return 0;
}

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
#define WINDOW_X 640
#define WINDOW_Y 1280
ShaderProgram program;
const float scale = 20.0f;
const float speed = scale;
float y_max;
float x_max;
TexturePool texAtlas;
void setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X, WINDOW_Y, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glViewport(0, 0, WINDOW_X, WINDOW_Y);

	y_max = scale;
	x_max = WINDOW_X / (float)WINDOW_Y * scale;
	projectionMatrix = glm::ortho(-x_max, x_max, -y_max, y_max, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);

	viewMatrix = glm::mat4(1.0f);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);
	//INIT TEXTURES
	texAtlas.addAtlas(std::string("assets/sheet.xml"));
	texAtlas.addSingle(std::string("assets/backgrounds/black.png"));
	//INIT ANIMATIONS
	setupAssets(texAtlas);
	//GAME OBJECTS
	playerSpaceship(0.0f, -0.8f * scale);
}

const Uint8 *keys = SDL_GetKeyboardState(NULL);
void update() {

	static float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	readInputs(elapsed);
	updateAnimations(elapsed);
	updateMotion(elapsed); 
	handleMapBounds(x_max, y_max);
}

void display() {

	//glClear(GL_COLOR_BUFFER_BIT);
	renderBackground(program, backgroundDark, x_max, y_max);

	updateTransforms();
	render(program);

	SDL_GL_SwapWindow(displayWindow);
}