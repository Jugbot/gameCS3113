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

SDL_Window* displayWindow;

GLuint LoadTexture(const char *);
void setup(void);
void draw(void);

int main(int argc, char *argv[])
{
	setup();

    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
		draw();
    }
    
    SDL_Quit();
    return 0;
}

glm::mat4 projectionMatrix;
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
ShaderProgram program;
GLuint textureEmojiCute;
GLuint textureEmojiEvil;
GLuint textureEmojiLaugh;
void setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glViewport(0, 0, 640, 360);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

	textureEmojiCute  = LoadTexture(RESOURCE_FOLDER"cute.gif");
	textureEmojiEvil  = LoadTexture(RESOURCE_FOLDER"evil.gif");
	textureEmojiLaugh = LoadTexture(RESOURCE_FOLDER"laugh.gif");
	
	projectionMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetModelMatrix(modelMatrix);

	glUseProgram(program.programID);
}

float vertices[] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
void drawTex(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

void drawBox() {
	drawTex(NULL);
}

void draw() {

	glClear(GL_COLOR_BUFFER_BIT);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	modelMatrix = glm::mat4(1.0f);
	program.SetModelMatrix(modelMatrix);
	drawBox();

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 0.0f, 0.0f));
	program.SetModelMatrix(modelMatrix);
	drawTex(textureEmojiCute);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f));
	program.SetModelMatrix(modelMatrix);
	drawTex(textureEmojiEvil);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f));
	program.SetModelMatrix(modelMatrix);
	drawTex(textureEmojiLaugh);


	/*float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(program.positionAttribute);*/

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