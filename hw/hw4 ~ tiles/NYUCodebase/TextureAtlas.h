#pragma once
#include <tinyxml2.h>
#include <iostream>
#include <map>
#include <vector>
#include "util.h"

struct VertexBufferSet {
	GLuint xyz_id;
	GLuint uv_id;
	GLuint tex_id;
	float w, h;
};

class TexturePool {
private:
	std::map<std::string, VertexBufferSet> vbos;
public:
	TexturePool() {}
	void addAtlas(std::string& xmlpath)
	{
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(xmlpath.c_str())) {
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

		std::string imgfile = std::string(doc.FirstChildElement("TextureAtlas")->Attribute("imagePath"));
		std::cout << "File name: " << imgfile << std::endl;

		std::cout << "Folder: " << folder << std::endl;
		std::string imgpath = folder + '/' + imgfile;
		std::cout << imgpath << std::endl;

		Image image = LoadTexture(imgpath.c_str());

		for (tinyxml2::XMLNode* tex = doc.FirstChild()->FirstChild(); tex; tex = tex->NextSibling())
		{
			tinyxml2::XMLElement* elem = tex->ToElement();
			std::string filename = std::string(elem->Attribute("name"));
			float x = elem->FloatAttribute("x") / image.w;
			float y = elem->FloatAttribute("y") / image.h;
			float w = elem->FloatAttribute("width") / image.w;
			float h = elem->FloatAttribute("height") / image.h;
			float realw = w * image.w;
			float realh = h * image.h;
			float aspect = realw / realh;
			VertexBufferSet vbo;
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
			vbo.w = aspect;
			vbo.h = 1.0f;
			vbos[filename] = vbo;
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
				VertexBufferSet vbo;
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
				vbo.w = aspect;
				vbo.h = 1.0f;
				vbos[*nameIter] = vbo;
				
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
	VertexBufferSet& operator[](std::string& str) {
		return vbos[str];
	}
	VertexBufferSet& operator[](char* str) {
		return (*this)[std::string(str)];
	}
};