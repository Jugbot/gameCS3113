#pragma once
#include <algorithm>
#include "ShaderProgram.h"

struct Rectangle {
	float vertices[12];
	int size = 12;
	float a;
	float r;
	float g;
	float b;

	Rectangle(float, float, float, float, float, float);
	Rectangle(float, float);
	Rectangle();
	void render(ShaderProgram&);
};
