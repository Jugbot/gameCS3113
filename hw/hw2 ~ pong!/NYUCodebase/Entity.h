#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Rectangle.h"



struct Entity
{
	glm::mat4 transform;
	float x, y;
	float w, h;
	float vel_x = 0.0f;
	float vel_y = 0.0f;
	float acc_x = 0.0f;
	float acc_y = 0.0f;
	Rectangle shape;

	Entity(float, float, float, float);
	Entity();
	void update(float);
	void draw(ShaderProgram&);
	bool checkCollision(Entity&);
};

