#pragma once
//#include "ShaderProgram.h"
#include "Services.h"
#include "assets.h"
//#include <cassert>
//#include "entt/entity/registry.hpp"


static float bulletspeed = 20.0f;
static float bullettimeout = 3.0f;

//PLAYER
auto newPlayer(float x, float y) {
	auto eid = entityRegistry.create();
	entityRegistry.assign<Transform>(eid, x, y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	entityRegistry.assign<Sprite>(eid, blueSpaceship.id, blueSpaceship.uv, blueSpaceship.coords);
	entityRegistry.assign<Collider>(eid, blueSpaceship.w, blueSpaceship.h);
	entityRegistry.assign<Health>(eid, 5);
	entityRegistry.assign<Player>(eid, SDL_SCANCODE_SPACE, SDL_SCANCODE_A, SDL_SCANCODE_D);
	return eid;
}

auto newBlueLaser(float x, float y) {
	auto eid = entityRegistry.create();
	entityRegistry.assign<Transform>(eid, x, y, 0.0f, bulletspeed, 0.0f, 0.0f, 1.0f);
	entityRegistry.assign<Sprite>(eid, normalLaserBlueData.id, normalLaserBlueData.uv, normalLaserBlueData.coords);
	entityRegistry.assign<Animation>(eid, &normalLaserBlue, ONCE, 0.1f);
	entityRegistry.assign<Collider>(eid, normalLaserBlueData.w, normalLaserBlueData.h);
	entityRegistry.assign<Laser>(eid, PLAYER);
	entityRegistry.assign<Lifespan>(eid, bullettimeout);
	return eid;
}
//ENEMY
auto newEnemy(float x, float y) {
	auto eid = entityRegistry.create();
	entityRegistry.assign<Transform>(eid, x, y, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f, glm::pi<float>());
	entityRegistry.assign<Sprite>(eid, redSpaceship.id, redSpaceship.uv, redSpaceship.coords);
	entityRegistry.assign<Collider>(eid, redSpaceship.w, redSpaceship.h);
	entityRegistry.assign<Health>(eid, 2);
	entityRegistry.assign<Enemy>(eid);
	return eid;
}

auto newRedLaser(float x, float y) {
	auto eid = entityRegistry.create();
	entityRegistry.assign<Transform>(eid, x, y, 0.0f, -bulletspeed / 2, 0.0f, 0.0f, 1.0f, glm::pi<float>());
	entityRegistry.assign<Sprite>(eid, normalLaserRedData.id, normalLaserRedData.uv, normalLaserRedData.coords);
	entityRegistry.assign<Animation>(eid, &normalLaserRed, ONCE, 0.1f);
	entityRegistry.assign<Collider>(eid, normalLaserRedData.w, normalLaserRedData.h);
	entityRegistry.assign<Laser>(eid, ENEMY);
	entityRegistry.assign<Lifespan>(eid, bullettimeout);
	return eid;
}
//CONVENIENCE
auto newLaser(float x, float y, Owner owner) {
	if (owner == PLAYER)
		return newBlueLaser(x, y);
	else
		return newRedLaser(x, y);
}
//BACKGROUND
auto background(float size) {
	auto eid = entityRegistry.create();
	entityRegistry.assign<Transform>(eid, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, size);
	entityRegistry.assign<Sprite>(eid, backgroundDark.id, backgroundDark.uv, backgroundDark.coords);
	return eid;
}
//TEXT
auto newLetter(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, char letter) {
	TextureData& tex = texAtlas[std::string(1, letter)];
	auto eid = registry.create();
	registry.assign<Transform>(eid, x, y, 0.0f, 0.0f, 0.0f, 0.0f, size);
	registry.assign<Sprite>(eid, tex.id, tex.uv, tex.coords);
	return eid;
}
void newText(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, std::string& text, float letterSpace = 0.0f) {
	static TextureData spacing = texAtlas["a"];
	float xoffset = spacing.w * size / 2 * (text.size() - 1);
	for (char letter : text) {
		if (letter != ' ')
			newLetter(registry, texAtlas, x - xoffset, y, size, letter);
		xoffset -= spacing.w * size;
	}
}
//MENU
auto newBlueMenuOption(float x, float y, float size, std::string& text) {
	auto eid = menuRegistry.create();
	menuRegistry.assign<Transform>(eid, x, y, 0.0f, 0.0f, 0.0f, 0.0f, size);
	menuRegistry.assign<Sprite>(eid, buttonBlue.id, buttonBlue.uv, buttonBlue.coords);
	menuRegistry.assign<Collider>(eid, buttonBlue.w, buttonBlue.h);
	//newText(x,y,size,text);
	return eid;
}
