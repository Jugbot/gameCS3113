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
#include "helpers.h"

#define FLIP_HORIZONTAL 8u
#define FLIP_VERTICAL 4u
#define FLIP_DIAGONAL 2u

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
								auto eid = registry.create();
								registry.assign<Transform>(eid, (float) x, (float) mapSize.y - y, 0.0f, 1.0f - 2.f * flip_x, 1.f - 2.f * flip_y);
								registry.assign<Tile>(eid, x, y);
								registry.assign<ColliderSet>(eid, &texturedata.boxes);
								auto& vbos = texturedata.vbos;
								if (vbos.size() > 1)
									registry.assign<Animation>(eid, &texturedata.vbos, &texturedata.delays);
								auto& s = vbos[0];
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
						auto& texdata = texturepool["hero_idle"];
						newPlayer(texdata, pos.x / tilesize.x, (map.getBounds().height - pos.y) / tilesize.y + 1.5f);

					}
					else if (objtype == "Enemy") {
						auto pos = obj.getPosition();
						auto& texdata = texturepool["enemy_idle"];
						newEnemy(texdata, pos.x / tilesize.x, (map.getBounds().height - pos.y) / tilesize.y + 1.5f);
					}
					else if (objtype == "Gate") {
						auto pos = obj.getPosition();
						auto box = obj.getAABB();
						for (auto& prop : obj.getProperties()) {
							if (prop.getName() == "to") {
								newGate(prop.getStringValue(),
									pos.x / tilesize.x,
									(map.getBounds().height - pos.y) / tilesize.y,
									box.width / tilesize.x,
									box.height / tilesize.y);
							}
						}
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