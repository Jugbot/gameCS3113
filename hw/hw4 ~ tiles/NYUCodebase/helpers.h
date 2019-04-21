#pragma once
#include "Services.h"
#include "assets.h"

//TEXT
auto newLetter(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, char letter) {
	SpriteCollider& tex = texAtlas[std::string(1, letter)];
	auto eid = registry.create();
	registry.assign<Transform>(eid, x, y, size);
	registry.assign<Sprite>(eid, tex.vbo.xyz_id, tex.vbo.uv_id, tex.vbo.tex_id);
	return eid;
}
void newText(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, std::string& text, float letterSpace = 0.0f) {
	static SpriteCollider spacing = texAtlas["a"];
	float xoffset = spacing.boxes[0].width * size / 2 * (text.size() - 1);
	for (char letter : text) {
		if (letter != ' ') 
			newLetter(registry, texAtlas, x - xoffset, y, size, letter);
		xoffset -= spacing.boxes[0].width * size;
	}
}