#pragma once
#include "assets.h"
#include "Services.h"



static std::vector<Collider> humanCollision{ {0.5f, 1.f} };

//TEXT
auto newLetter(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, char letter) {
	SpriteCollider& tex = texAtlas[std::string(1, letter)];
	auto eid = registry.create();
	registry.assign<Transform>(eid, x, y, 1.f, size, size, size);
	registry.assign<Sprite>(eid, tex.vbos[0].xyz_id, tex.vbos[0].uv_id, tex.vbos[0].tex_id);
	return eid;
}
void newText(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, const std::string& text, float letterSpace = 0.0f) {
	static SpriteCollider spacing = texAtlas["a"];
	float xoffset = spacing.boxes[0].width * size / 2 * (text.size() - 1);
	for (char letter : text) {
		if (letter != ' ') 
			newLetter(registry, texAtlas, x - xoffset, y, size, letter);
		xoffset -= spacing.boxes[0].width * size;
	}
}

auto newEntity(SpriteCollider& texdata, float x, float y) {
	auto eid = entityRegistry.create();
	entityRegistry.assign<Health>(eid, 1);
	entityRegistry.assign<Dynamic>(eid, 0.f, 0.f, 0.f, -20.f);
	entityRegistry.assign<Sprite>(eid, texdata.vbos[0].xyz_id, texdata.vbos[0].uv_id, texdata.vbos[0].tex_id);
	entityRegistry.assign<Animation>(eid, &texdata.vbos, &texdata.delays);
	entityRegistry.assign<Transform>(eid, x, y, 1.0f);
	return eid;
}

auto newPlayer(SpriteCollider& texdata, float x, float y) {
	auto eid = newEntity(texdata, x, y);
	entityRegistry.assign<ColliderSet>(eid, &humanCollision);
	entityRegistry.assign<Player>(eid);
	return eid;
}

auto newEnemy(SpriteCollider& texdata, float x, float y) {
	auto eid = newEntity(texdata, x, y);
	entityRegistry.assign<ColliderSet>(eid, &humanCollision);
	entityRegistry.assign<Enemy>(eid);
	return eid;
}