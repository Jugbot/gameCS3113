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
#include <SDL_mixer.h>

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
void updateMainMenu(void);
void displayMainMenu(void);
void updateGameOver(void);
void displayGameOver(void);

enum GameState { MainMenu, Game, GameOver };
static GameState gameState = MainMenu;
static unsigned char tabIndex = 0;
static float lastFrameTicks = 0.0f;

static bool done = false;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	printf("Debugging Window:\n");
	setup();

	SDL_Event event;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		update();
		display();
		switch (gameState) {
		case MainMenu:
			updateMainMenu();
			displayMainMenu();
			break;
		case GameOver:
			updateGameOver();
			displayGameOver();
			break;
		}
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
#define WINDOW_X 1280
#define WINDOW_Y 720
ShaderProgram program;
const float scale = 10.0f;
float y_max;
float x_max;
TexturePool texAtlas;
World world;
Mix_Chunk *music;
Mix_Chunk *death;
Mix_Chunk *win;

std::vector<entt::DefaultRegistry::entity_type> mainMenuElements;
std::vector<entt::DefaultRegistry::entity_type> gameOverElements;

void quitGame() {
	done = true;
}

void startGame() {
	gameState = Game;
	lastFrameTicks = (float)SDL_GetTicks() / 1000.0f;
}

void gotoLevel(const std::string lvl) {
	if (lvl == "win") {
		Mix_PlayChannel(0, win, -1);
	}
	entityRegistry.reset();
	world.Load(entityRegistry, texAtlas, "assets/"+lvl+".tmx");
	startGame();
}

void resetGame() {
	gotoLevel("room1");
}

void changeSelected(entt::DefaultRegistry& registry, std::vector<entt::DefaultRegistry::entity_type>& elements, int oldval, int newval) {
	size_t size = elements.size();
	auto& el = registry.get<UIElement>(elements[oldval % size]);
	el.selected = false;
	auto& el2 = registry.get<UIElement>(elements[newval % size]);
	el2.selected = true;
}

void handlePlayerLeft() {
	if (gameState == MainMenu) {
		changeSelected(menuRegistry, mainMenuElements, tabIndex, tabIndex - 1);
		tabIndex--;
	}
	else if (gameState == GameOver) {
		changeSelected(gameoverRegistry, gameOverElements, tabIndex, tabIndex - 1);
		tabIndex--;
	}
}

void handlePlayerRight() {
	if (gameState == MainMenu) {
		changeSelected(menuRegistry, mainMenuElements, tabIndex, tabIndex + 1);
		tabIndex++;
	}
	else if (gameState == GameOver) {
		changeSelected(gameoverRegistry, gameOverElements, tabIndex, tabIndex + 1);
		tabIndex++;
	}
}

void handlePlayerAction() {
	if (gameState == MainMenu) {
		size_t size = mainMenuElements.size();
		auto& el = menuRegistry.get<UIElement>(mainMenuElements[tabIndex % size]);
		el.action();
	}
	else if (gameState == GameOver) {
		size_t size = gameOverElements.size();
		auto& el = gameoverRegistry.get<UIElement>(gameOverElements[tabIndex % size]);
		el.action();
	}
}

void setup() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X, WINDOW_Y, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
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
	//SOUNDS
	music = Mix_LoadWAV("assets/audio/music_jungle.wav");
	death = Mix_LoadWAV("assets/audio/oof.wav");
	win = Mix_LoadWAV("assets/audio/congratulations.wav");
	Mix_VolumeChunk(music, 48);
	Mix_PlayChannel( 0, music, -1);
	/*Mix_Music *music = Mix_LoadMUS("assets/audio/music_jungle.wav");
	Mix_PlayMusic(music, -1);*/
	//FONT
	std::string line;
	std::ifstream ifs("assets/thickfont.txt");
	assert(ifs);
	char curr;
	std::vector<std::string> alphabet;
	while (ifs >> curr)
		alphabet.push_back(std::string(1, curr));
	texAtlas.addGrid("assets/thickfont.png", alphabet, 'z' - 'a' + 1, 4);
	//TEXTURES
	std::vector<std::pair<const std::string, unsigned int>> hero_animation{
			{"idle", 1},
			{"move", 6},
			{"up", 1},
			{"down", 1},
			{"die", 6}
	};
	texAtlas.addAnimations("assets/hero.png", "hero_", hero_animation, { 0.1f }, 15, 1);
	std::vector<std::pair<const std::string, unsigned int>> anims{
			{"idle", 4},
			{"move", 4},
			{"attack", 5},
			{"hurt", 4},
			{"die", 4}
	};
	texAtlas.addAnimations("assets/slime.png", "enemy_", {
			{"idle", 4},
			{"move", 4},
			{"attack", 5},
			{"hurt", 4},
			{"die", 4}
		}, { 0.1f }, 8, 3);
	//UI
	std::string title("CAVES of GALLET");
	newText(menuRegistry, texAtlas, 0.f, 4.f, x_max / title.size(), title);
	mainMenuElements.push_back(newUIElement(menuRegistry, texAtlas, 0.f, -2.f, x_max / 10.f, std::string("Start"), startGame));
	mainMenuElements.push_back(newUIElement(menuRegistry, texAtlas, 0.f, -6.f, x_max / 10.f, std::string("Exit"), quitGame));
	std::string message("D E A D");
	newText(gameoverRegistry, texAtlas, 0.f, 0.f, x_max / message.size(), message);
	gameOverElements.push_back(newUIElement(gameoverRegistry, texAtlas, -x_max / 4.f, -4.f, x_max / 10.f, std::string("Retry"), resetGame));
	gameOverElements.push_back(newUIElement(gameoverRegistry, texAtlas, x_max / 4.f, -4.f, x_max / 10.f, std::string("Exit"), quitGame));
	//WORLD
	world.Load(entityRegistry, texAtlas, "assets/room1.tmx");
}

#define TIMESTEP 1.f/30.f
void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;

	updateHealth();
	checkGate();
	checkOutOfBounds();
	deathCheck();
	updatePlayerCamera(viewMatrix);
	program.SetViewMatrix(viewMatrix);

	int count = 0;
	while (elapsed > TIMESTEP) {
		lastFrameTicks += TIMESTEP;
		elapsed = ticks - lastFrameTicks;
		if (gameState != MainMenu)
			updateMotion(TIMESTEP);
		count++;
	}
	enemyMovement(TIMESTEP*count);
	readInputs(TIMESTEP*count);
	updateAnimations(TIMESTEP*count);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	updateTransforms(entityRegistry);
	renderAll(entityRegistry, program);
}

void updateMainMenu() {
	program.SetViewMatrix(glm::mat4(1.f));
}

void displayMainMenu() {
	glClear(GL_DEPTH_BUFFER_BIT);
	updateTransforms(menuRegistry);
	renderAll(menuRegistry, program);
	renderUI(menuRegistry, program);
}

void updateGameOver() {
	program.SetViewMatrix(glm::mat4(1.f));
}

void displayGameOver() {
	glClear(GL_DEPTH_BUFFER_BIT);
	updateTransforms(gameoverRegistry);
	renderAll(gameoverRegistry, program);
	renderUI(gameoverRegistry, program);
}