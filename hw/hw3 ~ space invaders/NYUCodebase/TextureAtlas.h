#pragma once
#include "pugixml/pugixml.hpp"
#include <iostream>
#include <map>
#include <vector>
#include "util.h"

struct TextureData {
	float uv[12];
	float coords[12];
	float w, h;
	GLuint id;
};

class TexturePool {
private:
	std::map<std::string, TextureData> tex_uvs;
public:
	TexturePool() {}
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
		std::cout << "Load result: " << result.description() << "\nfile name: " << imgfile << std::endl;

		std::cout << "folder: " << folder << std::endl;
		std::string imgpath = folder + '/' + imgfile;
		std::cout << imgpath << std::endl;

		Image image = LoadTexture(imgpath.c_str());
		pugi::xml_node textures = doc.child("TextureAtlas");
		//pugi::xml_node textures = doc.child("TextureAtlas").child("SubTexture");

		for (pugi::xml_node tex = textures.first_child(); tex; tex = tex.next_sibling())
		{
			std::string filename = tex.attribute("name").as_string();
			float x = tex.attribute("x").as_float() / image.w;
			float y = tex.attribute("y").as_float() / image.h;
			float w = tex.attribute("width").as_float() / image.w;
			float h = tex.attribute("height").as_float() / image.h;
			float aspect = w / h;
			TextureData texture = {	{
					x		, y + h,
					x + w	, y + h,
					x + w	, y,
					x		, y + h,
					x + w	, y,
					x		, y		
				}, {
					-aspect / 2, -0.5f,
					aspect / 2, -0.5f,
					aspect / 2, 0.5f,
					-aspect / 2, -0.5f,
					aspect / 2, 0.5f,
					-aspect / 2, 0.5f
				},
				w, h, image.id
			};
			//std::cout << "<UV name:" << filename << " x:" << x << " y:" << y << " w:" << w << " h:" << h << " >\n";
			tex_uvs[filename] = texture;
		}
	}
	void addGrid(std::string& imgpath, std::vector<std::string>& names, int cols, int rows, int offset = 0) {
		Image image = LoadTexture(imgpath.c_str());
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
				float aspect = w / h;
				tex_uvs[*nameIter] = { 
					{
						px		, py + h,
						px + w	, py + h,
						px + w	, py,
						px		, py + h,
						px + w	, py,
						px		, py
					}, {
						-aspect / 2, -0.5f,
						aspect / 2, -0.5f,
						aspect / 2, 0.5f,
						-aspect / 2, -0.5f,
						aspect / 2, 0.5f,
						-aspect / 2, 0.5f
					},
					w, h, image.id
				};
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
	TextureData& operator[](std::string& str) {
		return tex_uvs[str];
	}
	TextureData& operator[](char* str) {
		return (*this)[std::string(str)];
	}
};