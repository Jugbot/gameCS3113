#pragma once
#include "Services.h"

//LASER
static AnimationSet normalLaserBlue;
static TextureData normalLaserBlueData;
static AnimationSet normalLaserRed;
static TextureData normalLaserRedData;
static AnimationSet normalLaserBlueImpact;
static TextureData normalLaserBlueImpactData;
static AnimationSet normalLaserRedImpact;
static TextureData normalLaserRedImpactData;
//SHIP
static TextureData blueSpaceship;
static TextureData redSpaceship;
//BACKGROUND
static TextureData backgroundDark;
//MENU
static TextureData buttonBlue;
static TextureData buttonRed;

void setupAssets(TexturePool& texAtlas) {
	//BLUE
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
	//RED
	normalLaserRedImpactData = texAtlas["laserRed08.png"];
	normalLaserRedImpact = {
			texAtlas["laserRed08.png"].uv,
			texAtlas["laserRed09.png"].uv
	};
	normalLaserRedData = texAtlas["laserRed02.png"];
	normalLaserRed = {
			texAtlas["laserRed02.png"].uv,
			texAtlas["laserRed06.png"].uv,
			texAtlas["laserRed04.png"].uv
	};
	redSpaceship = texAtlas["playerShip2_red.png"];
	//BACKGROUND
	backgroundDark = texAtlas["purple.png"];
	//MENU
	buttonBlue = texAtlas["buttonBlue.png"];
	buttonRed = texAtlas["buttonRed.png"];
	
}