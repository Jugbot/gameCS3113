#pragma once
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include "entt/entity/registry.hpp"
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include "util.h"
#include "Components.h"
#include "TextureAtlas.h"

#define FLIP_HORIZONTAL 8u
#define FLIP_VERTICAL 4u
#define FLIP_DIAGONAL 2u

static Collider playerCollision{ 0.5f, 1.f };

class World {
private:
	std::set<std::string> nameset;
public:
	void Load(entt::DefaultRegistry& registry, TexturePool& texturepool, std::string tmxpath) {
		tmx::Map map;
		map.load(tmxpath);
		const auto& layers = map.getLayers();

		for (const auto& layer : layers)
		{
			switch (layer->getType()) 
			{
			case tmx::Layer::Type::Tile: 
			{
				const auto tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
				const auto& tilesets = map.getTilesets();
				for (const auto& tileset : tilesets) {
					if (!nameset.count(tileset.getName()))
					{
						nameset.emplace(tileset.getName());
						texturepool.addTileset(tileset);
					}

					const auto& mapSize = map.getTileCount();
					const auto& tileIDs = tileLayer->getTiles();
					for (auto y = 0u; y < mapSize.y; ++y)
					{
						for (auto x = 0u; x < mapSize.x; ++x)
						{
							auto idx = y * mapSize.x + x;
							if (idx < tileIDs.size() && tileIDs[idx].ID >= tileset.getFirstGID()
								&& tileIDs[idx].ID < (tileset.getFirstGID() + tileset.getTileCount()))
							{
								auto flags = tileIDs[idx].flipFlags;
								bool flip_x = flags & FLIP_HORIZONTAL;
								bool flip_y = flags & FLIP_VERTICAL;
								auto& texturedata = texturepool[tileset.getName() + std::to_string(tileIDs[idx].ID - tileset.getFirstGID())];
								Sprite s = texturedata.vbo;
								auto eid = registry.create();
								registry.assign<Transform>(eid, (float) x, (float) mapSize.y - y, 0.0f, 1.0f - 2.f * flip_x, 1.f - 2.f * flip_y);
								ColliderSet boxes;
								for (Collider& c : texturedata.boxes)
									boxes.boxes.push_back(&c);
								registry.assign<Tile>(eid, x, y);
								registry.assign<ColliderSet>(eid, boxes.boxes);
								registry.assign<Sprite>(eid, s.xyz_id, s.uv_id, s.tex_id);
							}
						}
					}
				}
			}
				break;
			case tmx::Layer::Type::Object:
			{
				auto tilesize = map.getTileSize();
				const auto& objLayer = layer->getLayerAs<tmx::ObjectGroup>();
				for (const auto& obj : objLayer.getObjects()) {
					std::string objtype = obj.getType();
					if (objtype == "Player")
					{
						auto pos = obj.getPosition();
						auto& texdata = texturepool["hero0"];
						auto eid = registry.create();
						registry.assign<Player>(eid);
						registry.assign<Dynamic>(eid, 0.f, 0.f, 0.f, -20.f);
						registry.assign<Sprite>(eid, texdata.vbo.xyz_id, texdata.vbo.uv_id, texdata.vbo.tex_id);
						ColliderSet boxes = { {&playerCollision} };
						registry.assign<ColliderSet>(eid, boxes.boxes);
						registry.assign<Transform>(eid, pos.x/tilesize.x, pos.y/tilesize.y, 1.0f);

					}
				}
			}
				break;
			default:
				std::cout << "unknown type\n";
				break;
			}
			
		}
		
		
	}
};