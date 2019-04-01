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
#define WINDOW_Y 960
ShaderProgram program;
const float scale = 10.0f;
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
	texAtlas.addSingle(std::string("assets/backgrounds/purple.png")); 
	//FONT
	std::string line;
	std::ifstream ifs("assets/thickfont.txt");
	assert(ifs);
	char curr;
	std::vector<std::string> alphabet;
	while (ifs >> curr)
		alphabet.push_back(std::string(1, curr));
	texAtlas.addGrid(std::string("assets/thickfont.png"), alphabet, 'z' - 'a' + 1, 4);
	//INIT ANIMATIONS
	setupAssets(texAtlas);
	//GAME OBJECTS
	newPlayer(0.0f, -0.8f * scale);
	//MENU OBJECTS
	std::string title("SPACE INVADAAARS");
	newText(menuRegistry, texAtlas, 0.0f, y_max / 2, x_max * 2 / (title.size()+2), title);
	std::string message("Click to Start");
	float subSize = x_max / message.size();
	newText(menuRegistry, texAtlas, 0.0f, 0.0f, subSize, message);
	message = "Move: A/D";
	newText(menuRegistry, texAtlas, 0.0f, -y_max / 2, subSize, message);
	message = "Shoot: Space";
	newText(menuRegistry, texAtlas, 0.0f, -y_max / 2 - 1.0f, subSize, message);
}


void checkWave(float t_delta) {
	static float t_accummulator = 0.0f;
	static int enemyCount = 1;
	static const float waveDelay = 2.0f;
	if (!entityRegistry.size<Enemy>()) {
		t_accummulator += t_delta;
		if (t_accummulator > waveDelay) {
			t_accummulator = 0.0f;
			for (int _ = 0; _ < enemyCount; _++)
				newEnemy((rand() / (float)RAND_MAX * 2 - 1) * x_max, y_max);
			enemyCount++;
		}
	}
}

int playersAlive() {
	return entityRegistry.size<Player>();
}

void update() {
	static bool gameover = false;
	static float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	checkLaserHits();
	deathCheck();
	timeoutCheck(elapsed);
	readInputs(elapsed);
	enemyMovement(elapsed);
	updateAnimations(elapsed);
	updateMotion(elapsed); 
	handleMapBounds(x_max, y_max);
	checkWave(elapsed);
	if (!gameover && !playersAlive()) {
		gameover = true;
		std::string message("D E A D");
		newText(entityRegistry, texAtlas, 0.0f, 0.0f, x_max / message.size(), message);
	}
}

void display() {
	renderBackground(program, backgroundDark, x_max, y_max);
	updateTransforms(entityRegistry);
	render(entityRegistry, program);
	SDL_GL_SwapWindow(displayWindow);
}

void updateMenu() {
}

void displayMenu() {
	renderBackground(program, backgroundDark, x_max, y_max);
	updateTransforms(menuRegistry);
	render(menuRegistry, program);
	SDL_GL_SwapWindow(displayWindow);
}