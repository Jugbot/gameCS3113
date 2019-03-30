#pragma once 
#include "ShaderProgram.h"

struct SpriteSheet
{
private:
	float lastdelta = 0.0f;
public:
	unsigned int textureID;
	unsigned int spriteCountX;
	unsigned int spriteCountY;
	unsigned int index = 0;
	float framelen;

	SpriteSheet();
	SpriteSheet(unsigned int textureID, int spriteCountX, int spriteCountY, float framelen);

	virtual void Draw(ShaderProgram &program);
	virtual void Update(float);
};

SpriteSheet::SpriteSheet() {}
SpriteSheet::SpriteSheet(unsigned int textureID, int spriteCountX, int spriteCountY, float framelen) : framelen(framelen), spriteCountX(spriteCountX), spriteCountY(spriteCountY) {}

void SpriteSheet::Draw(ShaderProgram &program) {
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;

	float texCoords[] = {
	u, v + spriteHeight,
	u + spriteWidth, v,
	u, v,
	u + spriteWidth, v,
	u, v + spriteHeight,
	u + spriteWidth, v + spriteHeight
	};

	float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f };
}

void SpriteSheet::Update(float timedelta) {
	lastdelta += timedelta;
	while (lastdelta > framelen) {
		lastdelta -= framelen;
		index = (index+1) % 
	}
}