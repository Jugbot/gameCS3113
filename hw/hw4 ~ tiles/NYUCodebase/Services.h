#pragma once
#include "ShaderProgram.h"
#include "Components.h"
#include <cassert>
#include "entt/entity/registry.hpp"
#include "assets.h"

static entt::DefaultRegistry entityRegistry;
static entt::DefaultRegistry menuRegistry;

void updateMotion(float t_delta) {
	entityRegistry.view<Transform, Dynamic>().each([t_delta](auto entity, auto &transform, auto& dynamic) {
		if (dynamic.acc_x == 0.0f) {
			if (std::fabsf(dynamic.vel_x) > 0.01f) //Stickyness
				dynamic.acc_x = std::copysignf(dynamic.friction, -dynamic.vel_x);
			else
				dynamic.vel_x = 0;
		}
		transform.x += dynamic.vel_x * t_delta;
		transform.y += dynamic.vel_y * t_delta;
		dynamic.vel_x += dynamic.acc_x * t_delta;
		dynamic.vel_y += dynamic.acc_y * t_delta;
	});

}

bool checkCollision(const Collider& b1, const Transform& p1, const Collider& b2, const Transform& p2) {
	return (abs(p1.x - p2.x) < (b1.width + b2.width) / 2) &&
		(abs(p1.y - p2.y) < (b1.height + b2.height) / 2);
}

void updateAnimations(float t_delta) {
	entityRegistry.view<Animation, Sprite>().each([t_delta](auto entity, auto& animation, auto& sprite) {
		if ((animation.t_accumulator += t_delta) > animation.speed) {
			animation.t_accumulator -= animation.speed;
			animation.frame += 1;
			if (animation.pattern == REPEAT)
				animation.frame %= animation.frames->size();
			if (animation.pattern == ONCE && animation.frame >= animation.frames->size())
				animation.frame = animation.frames->size() - 1;
			sprite.uv_id = animation.frames->at(animation.frame);
		}
	});
}

const Uint8 *keys = SDL_GetKeyboardState(NULL);
void readInputs(float t_delta) {
	static float cooldown = 0.0f;
	static float shootCooldown = 0.1f;
	static float agility = 40.0f;
	static float speedLimit = 10.0f;
	entityRegistry.view<Dynamic, Player>().each([t_delta](auto entity, auto& dynamic, auto& player) {
		if (cooldown > shootCooldown) cooldown = shootCooldown;
		cooldown += t_delta;
		if (keys[player.fire] && cooldown > shootCooldown) {
			cooldown -= shootCooldown;
		}
		if (keys[player.left] == keys[player.right]) {
			dynamic.acc_x = 0;
		}
		else if (keys[player.left]) {
			dynamic.acc_x = -agility;
		}
		else if (keys[player.right]) {
			dynamic.acc_x = agility;
		}
		//TODO: make speed limit component
		if (dynamic.vel_x > speedLimit) dynamic.vel_x = speedLimit;
		if (dynamic.vel_x < -speedLimit) dynamic.vel_x = -speedLimit;
	});
}

void enemyMovement(float t_delta) {
	static float mintime = 0.7f;
	static float moveDelayScale = 1.0f;
	static float shootDelayScale = 2.0f;
	static float agility = 20.0f;
	static float speedLimit = 5.0f;
	entityRegistry.view<Dynamic, Enemy>().each([t_delta](auto entity, auto& dynamic, auto& enemy) {
		enemy.t_accumulator_move += t_delta;
		enemy.t_accumulator_shoot += t_delta;
		if (enemy.t_accumulator_move > enemy.nextMove * moveDelayScale + mintime) {
			dynamic.acc_x = (rand() % 3 - 1) * agility;
			enemy.t_accumulator_move -= enemy.nextMove;
			enemy.nextMove = rand() / (float)RAND_MAX;
		}
		if (enemy.t_accumulator_shoot > enemy.nextShoot * shootDelayScale + mintime) {
			enemy.t_accumulator_shoot -= enemy.nextShoot;
			enemy.nextShoot = rand() / (float)RAND_MAX;
		}

		if (dynamic.vel_x > speedLimit) dynamic.vel_x = speedLimit;
		if (dynamic.vel_x < -speedLimit) dynamic.vel_x = -speedLimit;
	});
}

void updateTransforms(entt::DefaultRegistry& registry) {
	registry.view<Transform>().each([](auto entity, auto &transform) {
		transform.transform = glm::translate(glm::mat4(1.0f), glm::vec3(transform.x, transform.y, 0.0f));
		transform.transform = glm::scale(transform.transform, glm::vec3(transform.scale, transform.scale, 1.0f));
		transform.transform = glm::rotate(transform.transform, transform.radians, glm::vec3(0.0f, 0.0f, 1.0f));
	});
}

void render(entt::DefaultRegistry& registry, ShaderProgram& sp) {
	registry.view<Sprite, Transform>().each([&](auto entity, auto& sprite, auto& transform) {
		sp.SetModelMatrix(transform.transform);

		glBindTexture(GL_TEXTURE_2D, sprite.tex_id);

		glBindBuffer(GL_ARRAY_BUFFER, sprite.xyz_id);
		glVertexAttribPointer(sp.positionAttribute, 2, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(sp.positionAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, sprite.uv_id);
		glVertexAttribPointer(sp.texCoordAttribute, 2, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(sp.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(sp.positionAttribute);
		glDisableVertexAttribArray(sp.texCoordAttribute);
	});
}

void deathCheck() {
	entityRegistry.view<Health>().each([&](auto entity, auto& health) {
		if (health.health <= 0)
			entityRegistry.destroy(entity);
		//transform.x = -1000.0f;
	});
}

void timeoutCheck(float t_delta) {
	entityRegistry.view<Lifespan>().each([&](auto entity, auto& lifespan) {
		if (lifespan.time > 0.0f)
			lifespan.time -= t_delta;
		else
			entityRegistry.destroy(entity);
	});
}


/*
void registerObj(vec3* buf_points, vec3* buf_colors, GLuint& buf_id, int size) {
	size *= sizeof(vec3);
	glGenBuffers(1, &buf_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_id);
	glBufferData(GL_ARRAY_BUFFER, size * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buf_points);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, buf_colors);
}
*/