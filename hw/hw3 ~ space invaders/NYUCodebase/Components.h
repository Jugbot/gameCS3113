#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "util.h"
#include "TextureAtlas.h"
#include <vector>
#include <string>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"


struct Transform
{
	float x, y;
	float vel_x, vel_y;
	float acc_x, acc_y;
	float scale = 1.0f;
	float radians = 0.0f;
	float z_distance = 0.0f;
	float friction = 5.0f;
	glm::mat4 transform = glm::mat4(1.0f);
};

//Physics or hit detection
struct Collider {
	float width, height;
};

//For rendering
struct Sprite
{
	GLuint texture;
	float* uv;
	float* coords;
};

//Slideshow animation
//Updates Graphics
typedef std::vector<float*> AnimationSet;
enum AnimationPattern { REPEAT, ONCE };
struct Animation
{
	AnimationSet* frames;
	AnimationPattern pattern;
	float speed;
	float t_accumulator = 0.0f;
	unsigned int frame = 0;
};

//Destructables
struct Health {
	int health;
};

struct Lifespan {
	float time;
};

struct Enemy {
	float nextMove = 0.0f;
	float nextShoot = 0.0f;
	float t_accumulator_move = 0.0f;
	float t_accumulator_shoot = 0.0f;
};

#include "SDL_scancode.h"
struct Player {
	SDL_Scancode fire;
	SDL_Scancode left;
	SDL_Scancode right;
};

enum Owner {PLAYER, ENEMY};
struct Laser {
	Owner owner;
	int damage = 1;
};
