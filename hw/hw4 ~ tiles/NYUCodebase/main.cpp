#include <time.h>
#include <iostream>
#include <string>
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
#include "TileMap.h"
#include "Services.h"
#include "helpers.h"

SDL_Window* displayWindow;

void setup(void);
void update(void);
void display(void);
void updateMenu(void);
void displayMenu(void);

enum GameState { MainMenu, Game };
static GameState gameState = MainMenu;

static float lastFrameTicks = 0.0f;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	printf("Debugging Window:\n");
	setup();

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && 
				event.button.button == SDL_BUTTON_LEFT && 
				gameState == MainMenu)
			{
				gameState = Game;
				lastFrameTicks = (float)SDL_GetTicks() / 1000.0f;
			}
		}
		switch (gameState) {
		case MainMenu:
			updateMenu();
			displayMenu();
			break;
		case Game:
			update();
			display();
			break;
		}
	}

	SDL_Quit();
	return 0;
}

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
#define WINDOW_X 640
#define WINDOW_Y 360
ShaderProgram program;
const float scale = 10.0f;
float y_max;
float x_max;
TexturePool texAtlas;
World world;
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
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_X, WINDOW_Y);
	glClearColor(33.f/255.f, 38.f/255.f,63.f/255.f, 1.0f);

	y_max = scale;
	x_max = WINDOW_X / (float)WINDOW_Y * scale;
	projectionMatrix = glm::ortho(-x_max, x_max, -y_max, y_max, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);

	viewMatrix = glm::mat4(1.0f);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);
	//INIT TEXTURES
	//texAtlas.addAtlas(std::string("assets/sheet.xml"));
	//texAtlas.addSingle(std::string("assets/backgrounds/purple.png")); 
	//FONT
	std::string line;
	std::ifstream ifs("assets/thickfont.txt");
	assert(ifs);
	char curr;
	std::vector<std::string> alphabet;
	while (ifs >> curr)
		alphabet.push_back(std::string(1, curr));
	texAtlas.addGrid("assets/thickfont.png", alphabet, 'z' - 'a' + 1, 4);
	std::vector<std::pair<const std::string, unsigned int>> anims{
			{"stand", 1},
			{"run", 6},
			{"up", 1},
			{"down", 1},
			{"die", 6}
	};
	texAtlas.addAnimations("assets/enemy.png", "enemy_", anims, { 0.1f }, 15, 1);
	texAtlas.addAnimations("assets/hero.png", "hero_", anims, { 0.1f }, 15, 1);
	world.Load(entityRegistry, texAtlas, "assets/room1.tmx");
}

#define TIMESTEP 1.f/60.f
void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;

	updateHealth();
	deathCheck();
	updatePlayerCamera(viewMatrix);
	program.SetViewMatrix(viewMatrix);

	while (elapsed > TIMESTEP) {
		lastFrameTicks += TIMESTEP;
		elapsed = ticks - lastFrameTicks;

		updateMotion(TIMESTEP);
		readInputs(TIMESTEP);
		enemyMovement(TIMESTEP);
		updateAnimations(TIMESTEP);
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	updateTransforms(entityRegistry);
	render(entityRegistry, program);
	SDL_GL_SwapWindow(displayWindow);
}

void updateMenu() {

}

void displayMenu() {
	updateTransforms(menuRegistry);
	render(menuRegistry, program);
	SDL_GL_SwapWindow(displayWindow);
}