#pragma once
#include "Services.h"
#include "assets.h"

//TEXT
auto newLetter(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, char letter) {
	VertexBufferSet& tex = texAtlas[std::string(1, letter)];
	auto eid = registry.create();
	registry.assign<Transform>(eid, x, y, size);
	registry.assign<Sprite>(eid, tex.xyz_id, tex.uv_id, tex.tex_id);
	return eid;
}
void newText(entt::DefaultRegistry& registry, TexturePool& texAtlas, float x, float y, float size, std::string& text, float letterSpace = 0.0f) {
	static VertexBufferSet spacing = texAtlas["a"];
	float xoffset = spacing.w * size / 2 * (text.size() - 1);
	for (char letter : text) {
		if (letter != ' ')
			newLetter(registry, texAtlas, x - xoffset, y, size, letter);
		xoffset -= spacing.w * size;
	}
}