#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Rectangle.cpp"



class Entity
{
public:
	glm::mat4 transform;
	float x, y;
	float w, h;
	float vel_x = 0.0f;
	float vel_y = 0.0f;
	float acc_x = 0.0f;
	float acc_y = 0.0f;
	Rectangle shape;

	Entity(float x, float y, float w, float h);
	Entity();

	virtual void Update(float);
	virtual void Draw(ShaderProgram&);
	bool checkCollision(Entity&);
};



Entity::Entity(float x, float y, float w, float h) : x(x), y(y), w(w), h(h), shape(w,h) {
	Entity::Update(0);
}
Entity::Entity() : Entity::Entity(0.0f, 0.0f, 1.0f, 1.0f) { }

void Entity::Draw(ShaderProgram &sp) {
	sp.SetModelMatrix(Entity::transform);
}

void Entity::Update(float t_delta) {
	Entity::x += Entity::vel_x * t_delta;
	Entity::y += Entity::vel_y * t_delta;
	Entity::vel_x += Entity::acc_x * t_delta;
	Entity::vel_y += Entity::acc_y * t_delta;
	Entity::transform = glm::mat4(1.0f);
	Entity::transform = glm::translate(Entity::transform, glm::vec3(Entity::x, Entity::y, 0.0f));
}

bool Entity::checkCollision(Entity& other) {
	return (abs(Entity::x - other.x) < (Entity::w + other.w) / 2) &&
		(abs(Entity::y - other.y) < (Entity::h + other.h) / 2);
}