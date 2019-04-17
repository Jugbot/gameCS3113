#pragma once
#include <Tmx.h>
#include <string>
#include <iostream>

class TileMap {
private:
	Tmx::Map map;
public:
	TileMap(std::string tmxpath) {
		map.ParseFile(tmxpath);
		if (map.HasError()) {
			std::cout << "tmx read error";
			std::abort();
		}


	}
};