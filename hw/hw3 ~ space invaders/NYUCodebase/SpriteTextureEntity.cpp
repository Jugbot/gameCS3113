#pragma once
#include "Entity.cpp"
#include "SpriteSheet.cpp"

class SpriteTextureEntity :
	public Entity
{
public:
	SpriteSheet spritesheet;

	SpriteTextureEntity();
	SpriteTextureEntity(SpriteSheet& spritesheet, float x, float y, float w, float h);
	SpriteTextureEntity(unsigned int textureID, int spriteCountX, int spriteCountY, float x, float y, float w, float h);

	virtual void Draw(ShaderProgram &program);
};

SpriteTextureEntity::SpriteTextureEntity(){}
SpriteTextureEntity::SpriteTextureEntity(SpriteSheet & spritesheet, float x, float y, float w, float h) : spritesheet(spritesheet), Entity(x, y, w, h) {}
SpriteTextureEntity::SpriteTextureEntity(unsigned int textureID, int spriteCountX, int spriteCountY, float x, float y, float w, float h) :
	spritesheet(textureID, spriteCountX, spriteCountY), Entity(x, y, w, h) {}

void SpriteTextureEntity::Draw(ShaderProgram &program)
{
	Entity::Draw(program);
	spritesheet.Draw(program);
}