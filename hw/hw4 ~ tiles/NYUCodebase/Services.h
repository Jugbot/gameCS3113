#pragma once
#include "ShaderProgram.h"
#include "Components.h"
#include <cassert>
#include "entt/entity/registry.hpp"
#include "assets.h"

static entt::DefaultRegistry entityRegistry;
static entt::DefaultRegistry menuRegistry;
static entt::DefaultRegistry gameoverRegistry;

//entt::connnect<Sprite>(entityRegistry.on_construct<Animation>());

bool checkRange(float p1, float r1, float p2, float r2) {
	return abs(p1 - p2) < (r1 + r2) / 2;
}


bool checkRangeOffsetScale(float x1, float off1, float r1, float s1,
						   float x2, float off2, float r2, float s2) {
	return checkRange(
		x1 + off1 * s1, abs(r1 * s1),
		x2 + off2 * s2, abs(r2 * s2)
	);
}

bool checkCollisionX(const Collider& b1, const Transform& p1, const Collider& b2, const Transform& p2, float margin = 0.f) {
	return checkRangeOffsetScale(p1.x, b1.offset_x, b1.width + margin, p1.scale_x,
								 p2.x, b2.offset_x, b2.width, p2.scale_x);
}

bool checkCollisionY(const Collider& b1, const Transform& p1, const Collider& b2, const Transform& p2, float margin = 0.f) {
	return checkRangeOffsetScale(p1.y, b1.offset_y, b1.height + margin, p1.scale_y,
								 p2.y, b2.offset_y, b2.height, p2.scale_y);
}

bool checkCollision(const Collider& b1, const Transform& p1, const Collider& b2, const Transform& p2, float margin = 0.f) {
	return checkCollisionX(b1, p1, b2, p2, margin) &&
		checkCollisionY(b1, p1, b2, p2, margin);
}

bool checkCollisions(const ColliderSet& b1, const Transform& p1, const ColliderSet& b2, const Transform& p2) {
	for (auto c1 : *b1.boxes) {
		for (auto c2 : *b2.boxes) {
			if (checkCollision(c1, p1, c2, p2))
				return true;
		}
	}
	return false;
}

#define PADDING 0.01f
float distance(float p1, float p2, float r1, float r2, float s1, float s2, float off1, float off2) {
	float pf1 = (p1 + off1 * s1);
	float pf2 = (p2 + off2 * s2);
	return (pf1 < pf2 ? -1 : 1) * (abs(pf1 - pf2) - (abs(r1 * s1) + abs(r2 * s2) + PADDING) / 2.f);
}

#define C_DOWN 1u
#define C_UP 2u
#define C_LEFT 4u
#define C_RIGHT 8u
unsigned char sweepCollision(const Transform& old_p1, Transform& new_p1, const ColliderSet& bs1, const Transform& p2, const ColliderSet& bs2) {
	unsigned char collisionFlags = 0;
	for (auto b2 : *bs2.boxes) {
		for (auto b1 : *bs1.boxes) {
			Transform p1 = old_p1;
			p1.x = new_p1.x;
			if (checkCollision(b1, p1, b2, p2)) {
				new_p1.x -= distance(p1.x, p2.x, b1.width, b2.width, p1.scale_x, p2.scale_x, b1.offset_x, b2.offset_x);
				collisionFlags |= (p1.x > p2.x) ? C_LEFT : C_RIGHT;
			}
			p1.x = old_p1.x;
			p1.y = new_p1.y;
			if (checkCollision(b1, p1, b2, p2)) {
				new_p1.y -= distance(p1.y, p2.y, b1.height, b2.height, p1.scale_y, p2.scale_y, b1.offset_y, b2.offset_y);
				collisionFlags |= (p1.y > p2.y) ? C_DOWN : C_UP;
			}
		}
	}
	return collisionFlags;

}

void updateMotion(float t_delta) {
	//auto& solids = entityRegistry.view<Tile, Transform, ColliderSet>();
	entityRegistry.view<Transform, Dynamic>().each([&, t_delta](auto entity, auto &transform, auto& dynamic) {
		Transform last_transform = transform;
		//Note the order of these operations make sure that an 
		//entity's down flag stays triggered every update
		dynamic.vel_x += dynamic.acc_x * t_delta;
		dynamic.vel_y += dynamic.acc_y * t_delta;
		transform.x += dynamic.vel_x * t_delta;
		transform.y += dynamic.vel_y * t_delta;
		if (entityRegistry.has<ColliderSet>(entity)) {
			ColliderSet& colliderset = entityRegistry.get<ColliderSet>(entity);
			entityRegistry.view<Tile, Transform, ColliderSet>().each([&](auto tile_entity, auto& tile, auto& tile_transform, auto& tile_colliderset) {
				unsigned char cflags = sweepCollision(last_transform, transform, colliderset, tile_transform, tile_colliderset);
				if (cflags & (C_UP | C_DOWN))
					dynamic.vel_y = 0.f;
				if (cflags & (C_LEFT | C_RIGHT))
					dynamic.vel_x = 0.f;
				colliderset.flag_up = cflags & C_UP || colliderset.flag_up;
				colliderset.flag_down = cflags & C_DOWN || colliderset.flag_down;
				colliderset.flag_left = cflags & C_LEFT || colliderset.flag_left;
				colliderset.flag_right = cflags & C_RIGHT || colliderset.flag_right;
				
			});
		}
	});

}

void updateHealth() {
	auto enemies = entityRegistry.view<Enemy, Transform, ColliderSet>();
	entityRegistry.view<Player, Transform, ColliderSet, Health>().each([&](auto pid, auto& player, auto& player_transform, auto& player_colliderset, auto& player_health) {
		enemies.each([&](auto eid, auto& enemy, auto& enemy_transform, auto& enemy_colliderset) {
			if (checkCollisions(player_colliderset, player_transform, enemy_colliderset, enemy_transform))
				player_health.health -= enemy.damage;
		});
	});
}

void updateAnimations(float t_delta) {
	entityRegistry.view<Animation, Sprite>().each([t_delta](auto entity, Animation& animation, auto& sprite) {
		if (animation.frames->size() && animation.delays->size() && 
			(animation.t_accumulator += t_delta) > animation.delays->at(animation.frame % animation.delays->size())) {
			animation.t_accumulator -= animation.delays->at(animation.frame % animation.delays->size());
			animation.frame += 1;
			if (animation.pattern == REPEAT)
				animation.frame %= animation.frames->size();
			if (animation.pattern == ONCE && animation.frame >= animation.frames->size())
				animation.frame = animation.frames->size() - 1;
			sprite = animation.frames->at(animation.frame);
		}
	});
}

const Uint8 *keys = SDL_GetKeyboardState(NULL);
void readInputs(float t_delta) {
	const static float agility = 20.0f;
	const static float speedLimit = 5.f;
	const static float friction = 20.f;
	const static float deadzone = 0.5f;
	entityRegistry.view<Transform, Dynamic, Player, ColliderSet, Animation, Health>().each([t_delta](auto entity, auto& transform, auto& dynamic, Player& player, auto& colliderset, auto& animation, auto& health) {
		if (health.status == DEAD) {
			if (dynamic.vel_x > deadzone)
				dynamic.acc_x = -friction;
			else if (dynamic.vel_x < -deadzone)
				dynamic.acc_x = friction;
			else
				dynamic.acc_x = dynamic.vel_x = 0.f;
		}
		if (gameState == Game) {
			if (player.t_accumulator > player.action_cooldown)
				player.t_accumulator = player.action_cooldown;
			player.t_accumulator += t_delta;
			if (keys[player.action] && player.t_accumulator > player.action_cooldown && colliderset.flag_down) {
				player.t_accumulator -= player.action_cooldown;
				dynamic.vel_y = 10.0f;
			}
			if (keys[player.left] == keys[player.right]) {
				if (dynamic.vel_x > deadzone)
					dynamic.acc_x = -friction;
				else if (dynamic.vel_x < -deadzone)
					dynamic.acc_x = friction;
				else
					dynamic.acc_x = dynamic.vel_x = 0.f;
			}
			else if (keys[player.left]) {
				dynamic.acc_x = -agility;
				transform.scale_x = -1.f;
			}
			else if (keys[player.right]) {
				dynamic.acc_x = agility;
				transform.scale_x = 1.f;
			}
			if (dynamic.vel_x > speedLimit) dynamic.vel_x = speedLimit;
			if (dynamic.vel_x < -speedLimit) dynamic.vel_x = -speedLimit;
			// Animations
			if (!colliderset.flag_down) {
				if (dynamic.vel_y > 0) {
					animation.frames = &texAtlas["hero_up"].vbos;
				}
				else {
					animation.frames = &texAtlas["hero_down"].vbos;
				}
			}
			else if (keys[player.left] || keys[player.right]) {
				animation.frames = &texAtlas["hero_move"].vbos;
			}
			else {
				animation.frames = &texAtlas["hero_idle"].vbos;
			}
			// Reset flags
			colliderset.flag_up = colliderset.flag_down = colliderset.flag_left = colliderset.flag_right = false;
		}
		// Global Callbacks
		static SDL_Scancode lastkey;
		if (keys[player.action]) {
			if (lastkey != player.action) {
				handlePlayerAction();
				lastkey = player.action;
			}
		}
		else if (keys[player.left]) {
			if (lastkey != player.left) {
				handlePlayerLeft();
				lastkey = player.left;
			}
		}
		else if (keys[player.right]) {
			if (lastkey != player.right) {
				handlePlayerRight();
				lastkey = player.right;
			}
		}
		else {
			lastkey = SDL_SCANCODE_UNKNOWN;
		}
	});
}

void enemyMovement(float t_delta) {
	const static float agility = 2.0f;
	entityRegistry.view<Transform, Dynamic, Enemy, ColliderSet>().each([t_delta](auto entity, auto& transform, auto& dynamic, auto& enemy, auto& colliderset) {
		if (colliderset.flag_left || dynamic.vel_x >= 0.f) {
			dynamic.vel_x = agility;
			transform.scale_x = 1.f;
		}
		if (colliderset.flag_right || dynamic.vel_x < 0.f) {
			dynamic.vel_x = -agility;
			transform.scale_x = -1.f;
		}
		colliderset.flag_up = colliderset.flag_down = colliderset.flag_left = colliderset.flag_right = false;
	});
}

void updateTransform(Transform& transform) {
	transform.transform = glm::translate(glm::mat4(1.0f), glm::vec3(transform.x, transform.y, transform.z));
	transform.transform = glm::scale(transform.transform, glm::vec3(transform.scale_x, transform.scale_y, 1.0f));
	transform.transform = glm::rotate(transform.transform, transform.radians, glm::vec3(0.0f, 0.0f, 1.0f));
}

void updateTransforms(entt::DefaultRegistry& registry) {
	registry.view<Transform>().each([](auto entity, auto &transform) {
		updateTransform(transform);
	});
}

void updatePlayerCamera(glm::mat4& viewMatrix) {
	entityRegistry.view<Player, Transform>().each([&](auto entity, auto& player, auto &transform) {
		viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-transform.x, -transform.y, 0.0f));
	});
}

void render(ShaderProgram& sp, Sprite& sprite, Transform& transform) {
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
}

void renderUI(entt::DefaultRegistry& registry, ShaderProgram& sp) {
	registry.view<UIElement>().each([&](auto entity, UIElement& uielement) {
		if (uielement.selected)
			sp.SetTint(1, 1, 0, 1);
		for (auto& t : uielement.renderGroup) {
			updateTransform(std::get<0>(t));
			render(sp, std::get<1>(t), std::get<0>(t));
		}
		sp.SetTint(0, 0, 0, 0);
	});
}

void renderAll(entt::DefaultRegistry& registry, ShaderProgram& sp) {
	registry.view<Sprite, Transform>().each([&](auto entity, auto& sprite, auto& transform) {
		render(sp, sprite, transform);
	});
}

void deathCheck() {
	entityRegistry.view<Health>().each([&](auto entity, auto& health) {
		if (health.health <= 0 && health.status == ALIVE) {
			if (entityRegistry.has<Player, Transform>(entity)) {
				auto& transform = entityRegistry.get<Transform>(entity);
				Animation& animation = entityRegistry.get<Animation>(entity);
				animation.frames = &texAtlas["hero_die"].vbos;
				animation.pattern = ONCE;
				gameState = GameOver;
				Mix_PlayChannel(-1, death, 0);
			}
			health.status = DEAD;
		}
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

void checkGate() {
	entityRegistry.view<Gate, Transform, ColliderSet>().each([&](auto entity, auto& gate, auto& gate_transform, auto& gate_colliderset) {
		entityRegistry.view<Player, Transform, ColliderSet>().each([&](auto entity, auto& player, auto& player_transform, auto& player_colliderset) {
			if (checkCollisions(gate_colliderset,gate_transform,player_colliderset,player_transform)) {
				gotoLevel(gate.to);
			}
		});
	});
}

void checkOutOfBounds() {
	entityRegistry.view<Player, Transform, Health>().each([&](auto entity, auto& player, auto &transform, auto &health) {
		if (transform.y < 0.f)
			health.health = 0;
	});
}