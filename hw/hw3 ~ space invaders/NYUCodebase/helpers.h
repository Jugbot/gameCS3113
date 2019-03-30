#pragma once
//#include "ShaderProgram.h"
#include "Services.h"
//#include <cassert>
#include "entt/entity/registry.hpp"

//LASER
static AnimationSet normalLaserBlue;
static TextureData normalLaserBlueData;
static AnimationSet normalLaserBlueImpact;
static TextureData normalLaserBlueImpactData;
//PLAYER
static TextureData blueSpaceship;
//BACKGROUND
static TextureData backgroundDark;

static float SCALE = 1.0f;

auto playerSpaceship(float x, float y) {
	auto eid = entityRegistery.create();
	entityRegistery.assign<Transform>(eid, x, y, 0.0f, 0.0f, 0.0f, 0.0f, SCALE);
	entityRegistery.assign<Sprite>(eid, blueSpaceship.id, blueSpaceship.uv, blueSpaceship.coords);
	entityRegistery.assign<Collider>(eid, blueSpaceship.w*SCALE, blueSpaceship.h*SCALE);
	entityRegistery.assign<Health>(eid, 5);
	entityRegistery.assign<Player>(eid, SDL_SCANCODE_SPACE, SDL_SCANCODE_A, SDL_SCANCODE_D);
	return eid;
}

auto blueLaserNormal(float x, float y, float vx, float vy) {
	auto eid = entityRegistery.create();
	entityRegistery.assign<Transform>(eid, x, y, vx, vy, 0.0f, 0.0f, SCALE);
	entityRegistery.assign<Sprite>(eid, normalLaserBlueData.id, normalLaserBlueData.uv, normalLaserBlueData.coords);
	entityRegistery.assign<Animation>(eid, &normalLaserBlue, ONCE, 0.1f);
	entityRegistery.assign<Collider>(eid, normalLaserBlueData.w*SCALE, normalLaserBlueData.h*SCALE);
	return eid;
}

auto background(float size) {
	auto eid = entityRegistery.create();
	entityRegistery.assign<Transform>(eid, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, size);
	entityRegistery.assign<Sprite>(eid, backgroundDark.id, backgroundDark.uv, backgroundDark.coords);
}

void setupAssets(TexturePool& texAtlas) {
	normalLaserBlueImpactData = texAtlas["laserBlue08.png"];
	normalLaserBlueImpact = {
			texAtlas["laserBlue08.png"].uv,
			texAtlas["laserBlue09.png"].uv
	};
	normalLaserBlueData = texAtlas["laserBlue03.png"];
	normalLaserBlue = {
			texAtlas["laserBlue03.png"].uv,
			texAtlas["laserBlue07.png"].uv,
			texAtlas["laserBlue05.png"].uv
	};
	blueSpaceship = texAtlas["playerShip1_blue.png"];

	backgroundDark = texAtlas["black.png"];
}

