#pragma once
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
#include "stb_image.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Rectangle.h"
#include "Entity.h"

SDL_Window* displayWindow;

GLuint LoadTexture(const char *);
void setup(void);
void update(void);
void display(void);
void initGameObjects(void);

Entity player1;
Entity player2;
Entity ball;
Rectangle score_back;
Rectangle score;
const float velocity_scale = 15.0f;

#define PLAYER2_UP SDL_SCANCODE_UP
#define PLAYER2_DOWN SDL_SCANCODE_DOWN
#define PLAYER1_UP SDL_SCANCODE_W
#define PLAYER1_DOWN SDL_SCANCODE_S

int main(int argc, char *argv[])
{
	setup();
	initGameObjects();

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
ShaderProgram programTextured;
ShaderProgram program;
#define WINDOW_X 640
#define WINDOW_Y 360
const float scale = 20.0f;
float y_max;
float x_max;
void setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X, WINDOW_Y, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	#ifdef _WINDOWS
		glewInit();
	#endif

	programTextured.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glViewport(0, 0, WINDOW_X, WINDOW_Y);

	y_max = scale;
	x_max = WINDOW_X / (float) WINDOW_Y * scale;
	projectionMatrix = glm::ortho(-x_max, x_max, -y_max, y_max, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);
	programTextured.SetProjectionMatrix(projectionMatrix);

	viewMatrix = glm::mat4(1.0f);
	program.SetViewMatrix(viewMatrix);
	programTextured.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);
	glUseProgram(programTextured.programID);
}

void initGameObjects() {
	static const float paddle_offset = 5.0f;
	ball = Entity(0.0f, 0.0f, 1.0f, 1.0f);
	ball.vel_y = ball.vel_x = sqrt(velocity_scale*velocity_scale/2);
	player1 = Entity(-(x_max - paddle_offset), 0.0f, 1.0f, 5.0f);
	player2 = Entity(x_max - paddle_offset, 0.0f, 1.0f, 5.0f);
	score = Rectangle(1.0f, 2.0f, 0.0f, 0.0f, 0.8f, 1.0f);
	score_back = Rectangle(x_max*2, 2.0f, 0.8f, 0.0f, 0.0f, 1.0f);
}

int checkWindowBoundsHeight(Entity& entity) {
	bool is_inside = (abs(entity.y) + entity.h / 2 < y_max);
	return (!is_inside) * entity.y;
}

int checkWin(Entity& entity) {
	bool is_inside = (abs(entity.x) + entity.w / 2 < x_max);
	return (!is_inside) * entity.x;
}

int player1_wins = 0;
int player2_wins = 0;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
void update() {

	int player1_constrain = checkWindowBoundsHeight(player1);
	if (player1_constrain < 0) {
		if (!keys[PLAYER1_UP])
			player1.vel_y = 0;
		player1.y = -(y_max - player1.h / 2);
	}
	else if (player1_constrain > 0) {
		if (!keys[PLAYER1_DOWN])
			player1.vel_y = 0;
		player1.y = (y_max - player1.h / 2);
	}

	if (keys[PLAYER1_UP]) {
		player1.vel_y = velocity_scale;
	}
	else if (keys[PLAYER1_DOWN]) {
		player1.vel_y = -velocity_scale;
	}
	if (keys[PLAYER1_UP] == keys[PLAYER1_DOWN])
		player1.vel_y = 0;

	int player2_constrain = checkWindowBoundsHeight(player2);
	if (player2_constrain < 0) {
		if (!keys[PLAYER2_UP])
			player2.vel_y = 0;
		player2.y = -(y_max - player2.h / 2);
	}
	else if (player2_constrain > 0) {
		if (!keys[PLAYER2_DOWN])
			player2.vel_y = 0;
		player2.y = (y_max - player2.h / 2);
	}

	if (keys[PLAYER2_UP]) {
		player2.vel_y = velocity_scale;
	}
	else if (keys[PLAYER2_DOWN]) {
		player2.vel_y = -velocity_scale;
	}
	if (keys[PLAYER2_UP] == keys[PLAYER2_DOWN])
		player2.vel_y = 0;

	if (ball.checkCollision(player1))
		ball.vel_x = abs(ball.vel_x);
	else if (ball.checkCollision(player2))
		ball.vel_x = -abs(ball.vel_x);

	int ball_constraint = checkWin(ball);
	if (ball_constraint > 0)
		player1_wins += 1;
	else if (ball_constraint < 0)
		player2_wins += 1;

	if (ball_constraint) 
		ball.x = ball.y = 0.0f;
	


	int ball_constrain = checkWindowBoundsHeight(ball);
	if (ball_constrain < 0)
		ball.vel_y = abs(ball.vel_y);
	else if (ball_constrain > 0)
		ball.vel_y = -abs(ball.vel_y);

	static float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	ball.update(elapsed);
	player1.update(elapsed);
	player2.update(elapsed);
}

void draw_score() {
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(0.0f, y_max - 1.0f, 0.0f));
	program.SetModelMatrix(transform);
	score_back.render(program);

	float ratio = (player1_wins + 1) / (float) (player2_wins + player1_wins + 2);
	transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3((ratio-1) * x_max, y_max - 1.0f, 0.0f));
	transform = glm::scale(transform, glm::vec3(2 * x_max * ratio, 1.0f, 1.0f));
	program.SetModelMatrix(transform);
	score.render(program);
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT);

	//program.SetProjectionMatrix(projectionMatrix);
	//program.SetViewMatrix(viewMatrix);
	draw_score();
	player1.draw(program);
	player2.draw(program);
	ball.draw(program);

	SDL_GL_SwapWindow(displayWindow);
}

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_image_free(image);
	return retTexture;
}