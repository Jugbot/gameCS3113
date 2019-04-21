#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <pugixml.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include "Components.h"
#include "util.h"

struct SpriteCollider {
	Sprite vbo;
	std::vector<Collider> boxes;
};

class TexturePool {
private:
	std::map<std::string, SpriteCollider> vbos;
public:
	TexturePool() {}
	void addTileset(const tmx::Tileset& tileset) {
		float unitsquare[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
		GLuint xyz_id = LoadArray(unitsquare, sizeof(unitsquare));
		Image texture = LoadTexture(tileset.getImagePath());
		auto& tilesize = tileset.getTileSize();
		GLuint tex_id = texture.id;
		std::string name = tileset.getName();
		for (const auto& tile : tileset.getTiles()) {
			std::vector<Collider> boxes;
			for (const tmx::Property& prop : tile.properties) {
				if (prop.getName() == "collision" && prop.getBoolValue()) {
					boxes.push_back({1.0f, 1.0f});
				}
			}
			for (const auto& collider : tile.objectGroup.getObjects()) {
				auto box = collider.getAABB();
				box.left += (box.width - tilesize.x) / 2.f;
				box.left /= tilesize.x;
				box.top += (box.height - tilesize.y) / 2.f;
				box.top /= tilesize.y;
				box.top = -box.top;
				box.width /= tilesize.x;
				box.height /= tilesize.y;
				boxes.push_back({ box.width, box.height, box.left, box.top });
			}

			auto size = tile.imageSize;
			auto pos = tile.imagePosition;
			float x = pos.x / (float) texture.w;
			float y = pos.y / (float) texture.h;
			float w = size.x / (float) texture.w; 
			float h = size.y / (float) texture.h;
			float uv[] = {
					x		, y + h,
					x + w	, y + h,
					x + w	, y,
					x		, y + h,
					x + w	, y,
					x		, y
			};
			GLuint uv_id = LoadArray(uv, sizeof(uv));
			Sprite vbo;
			vbo.tex_id = tex_id;
			vbo.uv_id = uv_id;
			vbo.xyz_id = xyz_id;
			vbos[tileset.getName() + std::to_string(tile.ID)] = { vbo, boxes };
		}
	}
	void addAtlas(std::string& xmlpath)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(xmlpath.c_str());
		if (!result) {
			std::cout << "xml read error.";
			std::abort();
		}

		std::string::size_type idx; 
		std::string folder = "";
		idx = xmlpath.find_last_of("/\\");
		if (idx != std::string::npos)
		{
			folder = xmlpath.substr(0,idx);
		}

		std::string imgfile = doc.child("TextureAtlas").attribute("imagePath").as_string();
		std::cout << "File name: " << imgfile << std::endl;

		std::cout << "Folder: " << folder << std::endl;
		std::string imgpath = folder + '/' + imgfile;
		std::cout << imgpath << std::endl;

		Image image = LoadTexture(imgpath.c_str());
		pugi::xml_node textures = doc.child("TextureAtlas");

		for (pugi::xml_node tex = textures.first_child(); tex; tex = tex.next_sibling()) 
		{
			std::string filename = tex.attribute("name").as_string();
			float x = tex.attribute("x").as_float() / image.w;
			float y = tex.attribute("y").as_float() / image.h;
			float w = tex.attribute("width").as_float() / image.w;
			float h = tex.attribute("height").as_float() / image.h;
			float realw = w * image.w;
			float realh = h * image.h;
			float aspect = realw / realh;
			Sprite vbo;
			Collider box;
			float uv[] = {
					x		, y + h,
					x + w	, y + h,
					x + w	, y,
					x		, y + h,
					x + w	, y,
					x		, y
			};
			vbo.uv_id = LoadArray(uv, sizeof(uv));
			float verts[] = {
					-aspect / 2, -0.5f,
					aspect / 2, -0.5f,
					aspect / 2, 0.5f,
					-aspect / 2, -0.5f,
					aspect / 2, 0.5f,
					-aspect / 2, 0.5f
			};
			vbo.xyz_id = LoadArray(verts, sizeof(verts));
			vbo.tex_id = image.id;
			box.width = aspect;
			box.height = 1.0f;
			vbos[filename] = { vbo, {box} };
		}
		std::cout << "Finished.\n";
	}
	void addGrid(std::string& imgpath, std::vector<std::string>& names, int cols, int rows, int offset = 0) {
		Image image = LoadTexture(imgpath.c_str());
		float w = 1.0f / cols * image.w;
		float h = 1.0f / rows * image.h;
		float aspect = w / h;
		float verts[] =
		{
				-aspect / 2, -0.5f,
				aspect / 2, -0.5f,
				aspect / 2, 0.5f,
				-aspect / 2, -0.5f,
				aspect / 2, 0.5f,
				-aspect / 2, 0.5f
		};
		GLuint xyz_id = LoadArray(verts, sizeof(verts));
		auto nameIter = names.begin();
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < cols; x++) {
				if (offset) {
					offset--; 
					continue;
				}
				if (nameIter == names.end())
					return;
				float w = 1.0f / cols;
				float h = 1.0f / rows;
				float px = x * w;
				float py = y * h;
				Sprite vbo;
				Collider box;
				float uv[] =
				{
					px		, py + h,
					px + w	, py + h,
					px + w	, py,
					px		, py + h,
					px + w	, py,
					px		, py
				};
				vbo.uv_id = LoadArray(uv, sizeof(uv));
				vbo.xyz_id = xyz_id;
				vbo.tex_id = image.id;
				box.width = aspect;
				box.height = 1.0f;
				vbos[*nameIter] = { vbo, {box} };
				
				nameIter++;
			}
		}
	}
	void addSingle(std::string& imgpath) {
		std::string imgname;
		size_t idx = imgpath.find_last_of("/\\");
		if (idx != std::string::npos)
		{
			imgname = imgpath.substr(idx+1);
		}
		addGrid(imgpath, std::vector<std::string>{ imgname }, 1, 1);
	}
	SpriteCollider& operator[](std::string& str) {
		return vbos[str];
	}
	SpriteCollider& operator[](char* str) {
		return (*this)[std::string(str)];
	}
};