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


struct Transform {
	float x, y, z;
	float scale_x = 1.0f;
	float scale_y = 1.0f;
	float scale_z = 1.0f;
	float radians = 0.0f;
	glm::mat4 transform = glm::mat4(1.0f);
};

struct Dynamic {
	float vel_x, vel_y;
	float acc_x, acc_y;
	float friction = 5.0f;
};

//Physics or hit detection
struct Collider {
	float width, height;
	float offset_x = 0.0f;
	float offset_y = 0.0f;
};

struct ColliderSet {
	std::vector<Collider*> boxes;
};

//For rendering
struct Sprite {
	GLuint xyz_id;
	GLuint uv_id;
	GLuint tex_id;
};

//Slideshow animation
//Updates Graphics
typedef std::vector<GLuint> AnimationSet;
enum AnimationPattern { REPEAT, ONCE };
struct Animation {
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
	int damage = 1;
};

#include "SDL_scancode.h"
struct Player {
	SDL_Scancode action = SDL_SCANCODE_SPACE;
	SDL_Scancode left = SDL_SCANCODE_A;
	SDL_Scancode right = SDL_SCANCODE_D;
	bool flag_up, flag_down, flag_left, flag_right;
};

struct Tile {
	unsigned int x, y;
};

enum Owner {PLAYER, ENEMY};
struct Laser {
	Owner owner;
	int damage = 1;
};
