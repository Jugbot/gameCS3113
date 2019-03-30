#pragma once
#include "ShaderProgram.h"
#include "Components.h"
#include <cassert>
#include "entt/entity/registry.hpp"

static entt::DefaultRegistry entityRegistery;


void updateMotion(float t_delta) {
	static float v_cap = 10.0f;
	entityRegistery.view<Transform>().each([t_delta](auto entity, auto &transform) {
		transform.x += transform.vel_x * t_delta;
		transform.y += transform.vel_y * t_delta;
		transform.vel_x += transform.acc_x * t_delta;
		transform.vel_y += transform.acc_y * t_delta;
		if (transform.vel_x > v_cap) transform.vel_x = v_cap;
		if (transform.vel_x < -v_cap) transform.vel_x = -v_cap;
		if (transform.vel_y > v_cap) transform.vel_y = v_cap;
		if (transform.vel_y < -v_cap) transform.vel_y = -v_cap;
	});

}

void handleMapBounds(float w, float h) {
	entityRegistery.view<Transform, Player>().each([w, h](auto entity, auto &transform, auto &player) {
		if (transform.x > w && transform.vel_x > 0)
			transform.x -= w*2;
		else if (transform.x < -w && transform.vel_x < 0)
			transform.x += w*2;
		if (transform.y > h && transform.vel_y > 0)
			transform.y -= h*2;
		else if (transform.y < -h && transform.vel_y < 0)
			transform.y += h*2;
	});
}

void updateTransforms() {
	entityRegistery.view<Transform>().each([](auto entity, auto &transform) {
		transform.transform = glm::translate(glm::mat4(1.0f), glm::vec3(transform.x, transform.y, 0.0f));
		transform.transform = glm::scale(transform.transform, glm::vec3(transform.scale, transform.scale, 1.0f));
		transform.transform = glm::rotate(transform.transform, transform.radians, glm::vec3(0.0f, 0.0f, 1.0f));
	});
}

void updateAnimations(float t_delta) {
	entityRegistery.view<Animation, Sprite>().each([t_delta](auto entity, auto& animation, auto& sprite) {
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

void readInputs(float t_delta) {
	entityRegistery.view<Transform, Player>().each([t_delta](auto entity, auto& transform, auto& player) {
		static float cooldown = 0.0f;
		if (cooldown > player.laserCooldown) cooldown = player.laserCooldown;
		cooldown += t_delta;
		if (keys[player.fire] && cooldown > player.laserCooldown) {
			cooldown -= player.laserCooldown;
			std::cout << cooldown << ", " << player.laserCooldown << std::endl;
			blueLaserNormal(transform.x, transform.y, 0.0f, speed);
		}
		if (keys[player.left] == keys[player.right]) {
			if (std::fabsf(transform.vel_x) > 0.01f) //Stickyness
				transform.acc_x = std::copysignf(transform.friction, -transform.vel_x);
			else
				transform.vel_x = 0;
		}
		else if (keys[player.left]) {
			transform.acc_x = -player.agility;
		}
		else if (keys[player.right]) {
			transform.acc_x = player.agility;
		}
	});
}

void render(ShaderProgram& sp) {
	entityRegistery.view<Sprite, Transform>().each([&](auto entity, auto& sprite, auto& transform) {
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
	entityRegistery.view<Health, Transform>().each([](auto entity, auto& health, auto& transform) {
		if (health.health <= 0)
			transform.x = -1000.0f;
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