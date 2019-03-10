#include "Entity.h"

Entity::Entity(float x, float y, float w, float h) : x(x), y(y), w(w), h(h), shape(w,h) { 
	Entity::update(0);
}
Entity::Entity() : Entity::Entity(0.0f, 0.0f, 1.0f, 1.0f) { }

void Entity::draw(ShaderProgram &sp) {
	sp.SetModelMatrix(Entity::transform);
	Entity::shape.render(sp);
}

void Entity::update(float t_delta) {
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