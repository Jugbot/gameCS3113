#pragma once
#include "ShaderProgram.h"
#include "Components.h"
#include <cassert>
#include "entt/entity/registry.hpp"
#include "assets.h"

static entt::DefaultRegistry entityRegistry;
static entt::DefaultRegistry menuRegistry;

void updateMotion(float t_delta) {
	entityRegistry.view<Transform>().each([t_delta](auto entity, auto &transform) {
		if (transform.acc_x == 0.0f) {
			if (std::fabsf(transform.vel_x) > 0.01f) //Stickyness
				transform.acc_x = std::copysignf(transform.friction, -transform.vel_x);
			else
				transform.vel_x = 0;
		}
		transform.x += transform.vel_x * t_delta;
		transform.y += transform.vel_y * t_delta;
		transform.vel_x += transform.acc_x * t_delta;
		transform.vel_y += transform.acc_y * t_delta;
	});

}

void handleMapBounds(float w, float h) {
	auto f = [w, h](auto entity, auto &transform, auto &player) {
		if (transform.x > w && transform.vel_x > 0)
			transform.x -= w * 2;
		else if (transform.x < -w && transform.vel_x < 0)
			transform.x += w * 2;
		if (transform.y > h && transform.vel_y > 0)
			transform.y -= h * 2;
		else if (transform.y < -h && transform.vel_y < 0)
			transform.y += h * 2;
	};
	//TODO: make speed limit component
	entityRegistry.view<Transform, Player>().each(f);
	entityRegistry.view<Transform, Enemy>().each(f);
}

bool checkCollision(const Collider& b1, const Transform& p1, const Collider& b2, const Transform& p2) {
	return (abs(p1.x - p2.x) < (b1.width + b2.width) / 2) &&
		(abs(p1.y - p2.y) < (b1.height + b2.height) / 2);
}

void checkLaserHits() { //better way to do this?
	auto players = entityRegistry.view<Player, Health, Collider, Transform>();
	auto enemies = entityRegistry.view<Enemy, Health, Collider, Transform>();
	auto collisionLogic = [](auto laser_entity, auto& laser_collider, auto& laser_transform, auto& laser,
							auto ship_entity, auto& ship_collider, auto& ship_transform, auto& ship_health) {
		if (checkCollision(laser_collider, laser_transform, ship_collider, ship_transform)) {
			if (entityRegistry.has<Laser>(laser_entity)) {
				laser_transform.vel_y = 0.0f;
				if (laser.owner == PLAYER) {
					entityRegistry.replace<Animation>(laser_entity, &normalLaserBlueImpact, ONCE, 0.25f);
					entityRegistry.replace<Lifespan>(laser_entity, 0.5f);
					entityRegistry.replace<Sprite>(laser_entity, normalLaserBlueImpactData.id, normalLaserBlueImpactData.uv, normalLaserBlueImpactData.coords);
				}
				else if (laser.owner == ENEMY) {
					entityRegistry.replace<Animation>(laser_entity, &normalLaserRedImpact, ONCE, 0.25f);
					entityRegistry.replace<Lifespan>(laser_entity, 0.5f);
					entityRegistry.replace<Sprite>(laser_entity, normalLaserRedImpactData.id, normalLaserRedImpactData.uv, normalLaserRedImpactData.coords);
				}
				ship_health.health -= laser.damage;
				entityRegistry.remove<Laser>(laser_entity);
			}
		}
	};
	entityRegistry.view<Transform, Laser, Collider, Animation>().each([&](auto entity, auto &transform, auto& laser, auto& collider, auto& animation) {
		if (laser.owner == PLAYER)
			enemies.each([&](auto entity2, auto &_, auto& health2, auto& collider2, auto& transform2) {
				collisionLogic(entity, collider, transform, laser, entity2, collider2, transform2, health2);
			});
		else if (laser.owner == ENEMY)
			players.each([&](auto entity2, auto &_, auto& health2, auto& collider2, auto& transform2) {
				collisionLogic(entity, collider, transform, laser, entity2, collider2, transform2, health2);
			});
	});
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
			sprite.uv = animation.frames->at(animation.frame);
		}
	});
}

const Uint8 *keys = SDL_GetKeyboardState(NULL);
void readInputs(float t_delta) {
	static float cooldown = 0.0f;
	static float shootCooldown = 0.1f;
	static float agility = 40.0f;
	static float speedLimit = 10.0f;
	entityRegistry.view<Transform, Player>().each([t_delta](auto entity, auto& transform, auto& player) {
		if (cooldown > shootCooldown) cooldown = shootCooldown;
		cooldown += t_delta;
		if (keys[player.fire] && cooldown > shootCooldown) {
			cooldown -= shootCooldown;
			//std::cout << cooldown << ", " << shootCooldown << std::endl;
			newBlueLaser(transform.x, transform.y);
		}
		if (keys[player.left] == keys[player.right]) {
			transform.acc_x = 0;
		}
		else if (keys[player.left]) {
			transform.acc_x = -agility;
		}
		else if (keys[player.right]) {
			transform.acc_x = agility;
		}
		//TODO: make speed limit component
		if (transform.vel_x > speedLimit) transform.vel_x = speedLimit;
		if (transform.vel_x < -speedLimit) transform.vel_x = -speedLimit;
	});
}

void enemyMovement(float t_delta) {
	static float mintime = 0.7f;
	static float moveDelayScale = 1.0f;
	static float shootDelayScale = 2.0f;
	static float agility = 20.0f;
	static float speedLimit = 5.0f;
	entityRegistry.view<Transform, Enemy>().each([t_delta](auto entity, auto& transform, auto& enemy) {
		enemy.t_accumulator_move += t_delta;
		enemy.t_accumulator_shoot += t_delta;
		if (enemy.t_accumulator_move > enemy.nextMove * moveDelayScale + mintime) {
			transform.acc_x = (rand() % 3 - 1) * agility;
			enemy.t_accumulator_move -= enemy.nextMove;
			enemy.nextMove = rand() / (float)RAND_MAX;
		}
		if (enemy.t_accumulator_shoot > enemy.nextShoot * shootDelayScale + mintime) {
			newRedLaser(transform.x, transform.y);
			enemy.t_accumulator_shoot -= enemy.nextShoot;
			enemy.nextShoot = rand() / (float)RAND_MAX;
		}

		if (transform.vel_x > speedLimit) transform.vel_x = speedLimit;
		if (transform.vel_x < -speedLimit) transform.vel_x = -speedLimit;
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

		glBindTexture(GL_TEXTURE_2D, sprite.texture);

		glVertexAttribPointer(sp.positionAttribute, 2, GL_FLOAT, false, 0, sprite.coords);
		glEnableVertexAttribArray(sp.positionAttribute);

		glVertexAttribPointer(sp.texCoordAttribute, 2, GL_FLOAT, false, 0, sprite.uv);
		glEnableVertexAttribArray(sp.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(sp.positionAttribute);
		glDisableVertexAttribArray(sp.texCoordAttribute);
	});
}

void renderBackground(ShaderProgram& sp, TextureData& texdata, float w, float h) {
	float scale = (w / h > texdata.w / texdata.h) ? w : h;
	scale *= 2;
	glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));
	sp.SetModelMatrix(transform);

	glBindTexture(GL_TEXTURE_2D, texdata.id);

	glVertexAttribPointer(sp.positionAttribute, 2, GL_FLOAT, false, 0, texdata.coords);
	glEnableVertexAttribArray(sp.positionAttribute);

	glVertexAttribPointer(sp.texCoordAttribute, 2, GL_FLOAT, false, 0, texdata.uv);
	glEnableVertexAttribArray(sp.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(sp.positionAttribute);
	glDisableVertexAttribArray(sp.texCoordAttribute);
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